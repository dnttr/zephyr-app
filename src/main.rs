use glfw::{Context, GlfwReceiver};
use std::{ffi::CString, ptr};
use gl::types::GLsizei;

fn main() {
    let mut glfw = glfw::init(glfw::fail_on_errors).unwrap();

    glfw.window_hint(glfw::WindowHint::ContextVersion(3, 3));
    glfw.window_hint(glfw::WindowHint::OpenGlProfile(glfw::OpenGlProfileHint::Core));

    #[cfg(target_os = "macos")]
    glfw.window_hint(glfw::WindowHint::OpenGlForwardCompat(true));

    let width = 800;
    let height = 600;

    let (mut window, events): (glfw::PWindow, GlfwReceiver<(f64, glfw::WindowEvent)>) =
    glfw.create_window(width, height, "Zephyr", glfw::WindowMode::Windowed)
            .expect("Failed to create GLFW window");

    window.make_current();
    window.set_key_polling(true);
    window.set_resizable(false);

    gl::load_with(|s| window.get_proc_address(s) as *const _);

    unsafe {
        gl::Viewport(0, 0, width as GLsizei, height as GLsizei);
        gl::ClearColor(0.2, 0.2, 0.2, 1.0);
    }

    while !window.should_close() {
        glfw.poll_events();

        unsafe {
            gl::Clear(gl::COLOR_BUFFER_BIT);
        }

        window.swap_buffers();
    }
}

unsafe fn compile_shader(src: &str, shader_type: u32) -> u32 {
    let shader = gl::CreateShader(shader_type);
    let c_str = CString::new(src).unwrap();
    gl::ShaderSource(shader, 1, &c_str.as_ptr(), ptr::null());
    gl::CompileShader(shader);

    let mut success = 0;
    gl::GetShaderiv(shader, gl::COMPILE_STATUS, &mut success);

    if success == 0 {
        let mut len = 0;
        gl::GetShaderiv(shader, gl::INFO_LOG_LENGTH, &mut len);
        let mut buffer: Vec<u8> = Vec::with_capacity(len as usize);
        buffer.set_len((len as usize) - 1);

        gl::GetShaderInfoLog(shader, len, ptr::null_mut(), buffer.as_mut_ptr() as *mut _);
        panic!("ERROR::SHADER::COMPILATION_FAILED\n{}", std::str::from_utf8(&buffer).unwrap());
    }

    shader
}