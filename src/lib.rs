use log;
use newrelic_telemetry::attribute::Value;
use newrelic_telemetry::span::SpanBatch;
use newrelic_telemetry::{blocking::Client, ClientBuilder};
use std::collections::HashMap;
use std::ffi::CStr;
use std::os::raw::c_char;
use std::ptr;

pub struct Span {}
type Attributes = HashMap<String, Value>;

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
    if attributes.is_null() || key.is_null() {
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
    if attributes.is_null() {
        return;
    }
    unsafe { Box::from_raw(*attributes) };
}

#[no_mangle]
pub extern "C" fn nrt_span_new(
    id: *const c_char,
    trace_id: *const c_char,
    parent_id: *const c_char,
) -> *mut Span {
    ptr::null_mut()
}

#[no_mangle]
pub extern "C" fn nrt_span_set_name(span: *mut Span, name: *const c_char) -> bool {
    false
}

#[no_mangle]
pub extern "C" fn nrt_span_set_service_name(span: *mut Span, service_name: *const c_char) -> bool {
    false
}

#[no_mangle]
pub extern "C" fn nrt_span_set_timestamp(span: *mut Span, timestamp: u64) -> bool {
    false
}

#[no_mangle]
pub extern "C" fn nrt_span_set_duration(span: *mut Span, duration: u64) -> bool {
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
pub extern "C" fn nrt_span_destroy(span: *mut *mut Span) {}

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
pub extern "C" fn nrt_client_new(key: *const c_char) -> *mut Client {
    if !key.is_null() {
        if let Ok(api_key) = unsafe { CStr::from_ptr(key).to_str() } {
            let result = ClientBuilder::new(api_key).build_blocking();
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
