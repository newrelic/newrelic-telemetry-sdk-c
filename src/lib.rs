use std::os::raw::c_char;
use std::ptr;

pub struct Sender {}
pub struct Span {}
pub struct SpanBatch {}

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
pub extern "C" fn nrt_sender_new(key: *const c_char) -> *mut Sender {
    ptr::null_mut()
}

#[no_mangle]
pub extern "C" fn nrt_sender_send(key: *const c_char, batch: *mut *mut SpanBatch) -> bool {
    false
}

#[no_mangle]
pub extern "C" fn nrt_sender_shutdown(sender: *mut *mut Sender) {}

#[no_mangle]
pub extern "C" fn nrt_sender_destroy(sender: *mut *mut Sender) {}
