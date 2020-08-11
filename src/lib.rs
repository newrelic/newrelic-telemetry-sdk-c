use std::os::raw::c_char;
use std::ptr;

pub struct Sender {}

#[no_mangle]
pub extern "C" fn nrt_sender_new(key: *const c_char) -> *mut Sender {
    return ptr::null_mut();
}

#[no_mangle]
pub extern "C" fn nrt_sender_shutdown(sender: *mut Sender) {}
