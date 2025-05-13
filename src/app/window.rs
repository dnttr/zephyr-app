use gl::types::{GLsizei, GLvoid};
use glfw::{Context, GlfwReceiver};
use std::time::Instant;
use std::{ffi::CString, ptr};
use super::utils::util::{get_rainbow};

use super::shaders::{create_shaders};

pub fn exec() {
    let mut glfw = glfw::init(glfw::fail_on_errors).unwrap();

    glfw.window_hint(glfw::WindowHint::ContextVersion(3, 3));
    glfw.window_hint(glfw::WindowHint::OpenGlProfile(glfw::OpenGlProfileHint::Core));

    #[cfg(target_os = "macos")]
    glfw.window_hint(glfw::WindowHint::OpenGlForwardCompat(true));

    glfw.window_hint(glfw::WindowHint::Samples(Some(16)));

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

    let vertices: [f32; 9] = [
        -0.5, -0.5, 0.0, // left
        0.5, -0.5, 0.0, // right
        0.0,  0.5, 0.0, // top
    ];

    let program = unsafe { create_shaders() };

    let color_location = unsafe {
        let c_str = CString::new("color").unwrap();
        gl::GetUniformLocation(program, c_str.as_ptr())
    };

    let (mut vao, mut vbo) = (0, 0);

    unsafe {
        gl::GenVertexArrays(1, &mut vao);
        gl::GenBuffers(1, &mut vbo);

        gl::BindVertexArray(vao);
        gl::BindBuffer(gl::ARRAY_BUFFER, vbo);

        gl::BufferData(gl::ARRAY_BUFFER, (vertices.len() * size_of::<f32>()) as isize, vertices.as_ptr() as *const GLvoid, gl::STATIC_DRAW);

        gl::VertexAttribPointer(0, 3, gl::FLOAT, gl::FALSE, 3 * size_of::<f32>() as i32, ptr::null());
        gl::EnableVertexAttribArray(0);
    }

    let time = Instant::now();

    unsafe { gl::Enable(gl::MULTISAMPLE) }

    while !window.should_close() {
        let elapsed = time.elapsed().as_secs_f32();
        let hue = (elapsed * 10.0) % 360.0;
        let (r, g, b) = get_rainbow(hue);

        glfw.poll_events();

        unsafe {
            gl::Clear(gl::COLOR_BUFFER_BIT);
            gl::UseProgram(program);
            gl::Uniform3f(color_location, r, g, b);
            gl::BindVertexArray(vao);
            gl::DrawArrays(gl::TRIANGLES, 0, 3);
        }

        window.swap_buffers();
    }

    unsafe {
        gl::DeleteVertexArrays(1, &mut vao);
        gl::DeleteBuffers(1, &mut vbo);
        gl::DeleteProgram(program);
    }
}
