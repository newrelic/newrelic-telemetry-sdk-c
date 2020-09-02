use log;
use newrelic_telemetry::attribute::Value;
use newrelic_telemetry::span::Span;
use newrelic_telemetry::span::SpanBatch;
use newrelic_telemetry::{blocking::Client, ClientBuilder};
use simplelog::{Config, LevelFilter, TermLogger, TerminalMode, WriteLogger};
use std::collections::HashMap;
use std::ffi::CStr;
use std::fs::File;
use std::os::raw::c_char;
use std::ptr;
use std::time::Duration;

pub struct ClientConfig {
    key: String,
    backoff_factor: Option<Duration>,
    retries_max: Option<u32>,
    host: Option<String>,
    port: Option<u16>,
    product: Option<String>,
    version: Option<String>,
    queue_max: Option<usize>,
}
type Attributes = HashMap<String, Value>;

impl From<&ClientConfig> for ClientBuilder {
    fn from(config: &ClientConfig) -> ClientBuilder {
        let mut builder = ClientBuilder::new(&config.key);

        if let Some(backoff_factor) = config.backoff_factor {
            builder = builder.backoff_factor(backoff_factor);
        }
        if let Some(retries_max) = config.retries_max {
            builder = builder.retries_max(retries_max);
        }
        if let Some(host) = &config.host {
            builder = builder.endpoint_traces(&host, config.port);
        }
        if let Some(product) = &config.product {
            builder =
                builder.product_info(&product, config.version.as_ref().unwrap_or(&"".to_string()));
        }
        if let Some(queue_max) = config.queue_max {
            builder = builder.blocking_queue_max(queue_max);
        }

        builder
    }
}

#[no_mangle]
pub extern "C" fn nrt_log_init(level: i32, filename: *const c_char) -> bool {
    if filename.is_null() {
        return false;
    }

    let level = match level {
        0 => LevelFilter::Error,
        1 => LevelFilter::Warn,
        2 => LevelFilter::Info,
        3 => LevelFilter::Debug,
        4 => LevelFilter::Trace,
        _ => LevelFilter::Off,
    };

    if let Ok(filename) = unsafe { CStr::from_ptr(filename).to_str() } {
        match filename {
            "stdout" => {
                if let Ok(_) = TermLogger::init(level, Config::default(), TerminalMode::Stdout) {
                    return true;
                }
            }
            "stderr" => {
                if let Ok(_) = TermLogger::init(level, Config::default(), TerminalMode::Stderr) {
                    return true;
                }
            }
            filename => {
                if let Ok(file) = File::create(filename) {
                    if let Ok(_) = WriteLogger::init(level, Config::default(), file) {
                        return true;
                    }
                }
                return false;
            }
        }
    }

    false
}

#[no_mangle]
pub extern "C" fn nrt_attributes_new() -> *mut Attributes {
    let attrs: HashMap<String, Value> = HashMap::new();
    Box::into_raw(Box::new(attrs))
}

fn nrt_attributes_set<T: Into<Value>>(
    attributes: *mut Attributes,
    key: *const c_char,
    value: T,
) -> bool {
    if key.is_null() {
        return false;
    }

    if let Some(attrs) = unsafe { attributes.as_mut() } {
        if let Ok(key) = unsafe { CStr::from_ptr(key).to_str() } {
            attrs.insert(key.to_string(), value.into());
            return true;
        }
    }

    false
}

#[no_mangle]
pub extern "C" fn nrt_attributes_set_int(
    attributes: *mut Attributes,
    key: *const c_char,
    value: i64,
) -> bool {
    nrt_attributes_set(attributes, key, value)
}

#[no_mangle]
pub extern "C" fn nrt_attributes_set_uint(
    attributes: *mut Attributes,
    key: *const c_char,
    value: u64,
) -> bool {
    nrt_attributes_set(attributes, key, value)
}

#[no_mangle]
pub extern "C" fn nrt_attributes_set_double(
    attributes: *mut Attributes,
    key: *const c_char,
    value: f64,
) -> bool {
    nrt_attributes_set(attributes, key, value)
}

#[no_mangle]
pub extern "C" fn nrt_attributes_set_string(
    attributes: *mut Attributes,
    key: *const c_char,
    value: *const c_char,
) -> bool {
    if value.is_null() {
        return false;
    }

    if let Ok(value) = unsafe { CStr::from_ptr(value).to_str() } {
        nrt_attributes_set(attributes, key, value)
    } else {
        false
    }
}

#[no_mangle]
pub extern "C" fn nrt_attributes_set_bool(
    attributes: *mut Attributes,
    key: *const c_char,
    value: bool,
) -> bool {
    nrt_attributes_set(attributes, key, value)
}

#[no_mangle]
pub extern "C" fn nrt_attributes_destroy(attributes: *mut *mut Attributes) {
    if !attributes.is_null() {
        let a = unsafe { *attributes };
        if !a.is_null() {
            let a = unsafe { Box::from_raw(a) };
            drop(a);
            unsafe { *attributes = ptr::null_mut() };
        }
    }
}

#[no_mangle]
pub extern "C" fn nrt_client_config_new(key: *const c_char) -> *mut ClientConfig {
    if key.is_null() {
        return ptr::null_mut();
    }

    if let Ok(key) = unsafe { CStr::from_ptr(key).to_str() } {
        let config = ClientConfig {
            key: key.to_string(),
            backoff_factor: None,
            retries_max: None,
            host: None,
            port: None,
            product: None,
            version: None,
            queue_max: None,
        };
        return Box::into_raw(Box::new(config));
    }

    ptr::null_mut()
}

#[no_mangle]
pub extern "C" fn nrt_client_config_set_backoff_factor(
    config: *mut ClientConfig,
    backoff_factor: u64,
) {
    if let Some(config) = unsafe { config.as_mut() } {
        config.backoff_factor = Some(Duration::from_millis(backoff_factor));
    }
}

#[no_mangle]
pub extern "C" fn nrt_client_config_set_retries_max(config: *mut ClientConfig, retries: u32) {
    if let Some(config) = unsafe { config.as_mut() } {
        config.retries_max = Some(retries);
    }
}

#[no_mangle]
pub extern "C" fn nrt_client_config_set_endpoint_traces(
    config: *mut ClientConfig,
    host: *const c_char,
    port: u16,
) {
    if host.is_null() {
        return;
    }

    if let Some(config) = unsafe { config.as_mut() } {
        if let Ok(host) = unsafe { CStr::from_ptr(host).to_str() } {
            config.host = Some(host.to_string());
            config.port = if port == 0 { None } else { Some(port) };
        }
    }
}

#[no_mangle]
pub extern "C" fn nrt_client_config_set_product_info(
    config: *mut ClientConfig,
    product: *const c_char,
    version: *const c_char,
) {
    if product.is_null() {
        return;
    }

    if let Some(config) = unsafe { config.as_mut() } {
        if let Ok(product) = unsafe { CStr::from_ptr(product).to_str() } {
            config.product = Some(product.to_string());
        }

        if !version.is_null() {
            if let Ok(version) = unsafe { CStr::from_ptr(version).to_str() } {
                config.version = Some(version.to_string());
            }
        }
    }
}

#[no_mangle]
pub extern "C" fn nrt_client_config_set_queue_max(config: *mut ClientConfig, queue_max: usize) {
    if let Some(config) = unsafe { config.as_mut() } {
        config.queue_max = Some(queue_max);
    }
}

#[no_mangle]
pub extern "C" fn nrt_client_config_destroy(config: *mut *mut ClientConfig) {
    if !config.is_null() {
        let c = unsafe { *config };
        if !c.is_null() {
            let c = unsafe { Box::from_raw(c) };
            drop(c);
            unsafe { *config = ptr::null_mut() };
        }
    }
}

#[no_mangle]
pub extern "C" fn nrt_span_new(
    id: *const c_char,
    trace_id: *const c_char,
    timestamp: u64,
) -> *mut Span {
    if !trace_id.is_null() {
        if let Ok(id) = unsafe { CStr::from_ptr(id).to_str() } {
            if let Ok(trace_id) = unsafe { CStr::from_ptr(trace_id).to_str() } {
                let span = Span::new(id, trace_id, timestamp);
                return Box::into_raw(Box::new(span));
            }
        }
    }

    ptr::null_mut()
}

#[no_mangle]
pub extern "C" fn nrt_span_set_id(span: *mut Span, id: *const c_char) -> bool {
    if !id.is_null() {
        if let Some(span) = unsafe { span.as_mut() } {
            if let Ok(id) = unsafe { CStr::from_ptr(id).to_str() } {
                span.set_id(id);
                return true;
            }
        }
    }
    false
}

#[no_mangle]
pub extern "C" fn nrt_span_set_trace_id(span: *mut Span, trace_id: *const c_char) -> bool {
    if !trace_id.is_null() {
        if let Some(span) = unsafe { span.as_mut() } {
            if let Ok(trace_id) = unsafe { CStr::from_ptr(trace_id).to_str() } {
                span.set_trace_id(trace_id);
                return true;
            }
        }
    }
    false
}

#[no_mangle]
pub extern "C" fn nrt_span_set_timestamp(span: *mut Span, timestamp: u64) -> bool {
    if let Some(span) = unsafe { span.as_mut() } {
        span.set_timestamp(timestamp);
        return true;
    }
    false
}

#[no_mangle]
pub extern "C" fn nrt_span_set_name(span: *mut Span, name: *const c_char) -> bool {
    if !name.is_null() {
        if let Some(span) = unsafe { span.as_mut() } {
            if let Ok(name) = unsafe { CStr::from_ptr(name).to_str() } {
                span.set_name(name);
                return true;
            }
        }
    }
    false
}

#[no_mangle]
pub extern "C" fn nrt_span_set_duration(span: *mut Span, duration: u64) -> bool {
    if let Some(span) = unsafe { span.as_mut() } {
        span.set_duration(Duration::from_millis(duration));
        return true;
    }
    false
}

#[no_mangle]
pub extern "C" fn nrt_span_set_parent_id(span: *mut Span, parent_id: *const c_char) -> bool {
    if !parent_id.is_null() {
        if let Some(span) = unsafe { span.as_mut() } {
            if let Ok(parent_id) = unsafe { CStr::from_ptr(parent_id).to_str() } {
                span.set_parent_id(parent_id);
                return true;
            }
        }
    }
    false
}

#[no_mangle]
pub extern "C" fn nrt_span_set_service_name(span: *mut Span, service_name: *const c_char) -> bool {
    if !service_name.is_null() {
        if let Some(span) = unsafe { span.as_mut() } {
            if let Ok(service_name) = unsafe { CStr::from_ptr(service_name).to_str() } {
                span.set_service_name(service_name);
                return true;
            }
        }
    }
    false
}

#[no_mangle]
pub extern "C" fn nrt_span_set_attributes(
    span: *mut Span,
    attributes: *mut *mut Attributes,
) -> bool {
    if attributes.is_null() {
        false
    } else if span.is_null() {
        nrt_attributes_destroy(attributes);
        return false;
    } else {
        false
    }
}

#[no_mangle]
pub extern "C" fn nrt_span_destroy(span: *mut *mut Span) {
    if let Some(s) = unsafe { span.as_mut() } {
        if !s.is_null() {
            let s = unsafe { Box::from_raw(s) };
            drop(s);
            unsafe { *span = ptr::null_mut() };
        }
    }
}

#[no_mangle]
pub extern "C" fn nrt_span_batch_new() -> *mut SpanBatch {
    ptr::null_mut()
}

#[no_mangle]
pub extern "C" fn nrt_span_batch_record(batch: *mut SpanBatch, span: *mut *mut Span) -> bool {
    true
}

#[no_mangle]
pub extern "C" fn nrt_span_batch_destroy(batch: *mut *mut SpanBatch) {}

#[no_mangle]
pub extern "C" fn nrt_client_new(cfg: *mut *mut ClientConfig) -> *mut Client {
    if !cfg.is_null() {
        if let Some(config) = unsafe { (*cfg).as_ref() } {
            let result = Into::<ClientBuilder>::into(config).build_blocking();
            nrt_client_config_destroy(cfg);
            match result {
                Ok(client) => return Box::into_raw(Box::new(client)),
                Err(err) => {
                    log::error!("unable to create client: {}", err.to_string());
                }
            }
        }
    }
    ptr::null_mut()
}

#[no_mangle]
pub extern "C" fn nrt_client_send(client: *mut Client, batch: *mut *mut SpanBatch) -> bool {
    if let Some(client) = unsafe { client.as_mut() } {
        if let Some(b) = unsafe { batch.as_mut() } {
            if !b.is_null() {
                client.send_spans(unsafe { *Box::from_raw(*b) });
                unsafe { *batch = ptr::null_mut() };
                return true;
            }
        }
    }
    false
}

#[no_mangle]
pub extern "C" fn nrt_client_shutdown(client: *mut *mut Client) {
    if !client.is_null() {
        let c = unsafe { *client };
        if !c.is_null() {
            let c = unsafe { Box::from_raw(c) };
            c.shutdown();
            unsafe { *client = ptr::null_mut() };
        }
    }
}

#[no_mangle]
pub extern "C" fn nrt_client_destroy(client: *mut *mut Client) {
    if !client.is_null() {
        let c = unsafe { *client };
        if !c.is_null() {
            let c = unsafe { Box::from_raw(c) };
            drop(c);
            unsafe { *client = ptr::null_mut() };
        }
    }
}
