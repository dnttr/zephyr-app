use gl::types::{GLsizei, GLvoid};
use glfw::{Context, GlfwReceiver};
use palette::{FromColor, Hsv, Srgb};
use std::{ffi::CString, ptr};

const VERTEX_SHADER_SOURCE: &str = r#"
#version 330 core
layout (location = 0) in vec3 aPos;
void main() {
    gl_Position = vec4(aPos, 1.0);
}
"#;

const FRAGMENT_SHADER_SOURCE: &str = r#"
#version 330 core
out vec4 FragColor;
void main() {
    FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}
"#;

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

    let vertices: [f32; 9] = [
        -0.5, -0.5, 0.0, // left
        0.5, -0.5, 0.0, // right
        0.0,  0.5, 0.0, // top
    ];

    let program = unsafe { create_shaders() };

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

    while !window.should_close() {
        glfw.poll_events();

        unsafe {
            gl::Clear(gl::COLOR_BUFFER_BIT);
            gl::UseProgram(program);
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

unsafe fn create_shaders() -> u32 {
    let vertex_shader = compile_shader(VERTEX_SHADER_SOURCE, gl::VERTEX_SHADER);
    let fragment_shader = compile_shader(FRAGMENT_SHADER_SOURCE, gl::FRAGMENT_SHADER);

    let shader_program = gl::CreateProgram();

    gl::AttachShader(shader_program, vertex_shader);
    gl::AttachShader(shader_program, fragment_shader);

    gl::LinkProgram(shader_program);

    let mut success = 0;
    gl::GetProgramiv(shader_program, gl::LINK_STATUS, &mut success);

    if success == 0 {
        let mut len = 0;
        gl::GetProgramiv(shader_program, gl::INFO_LOG_LENGTH, &mut len);

        let mut buffer: Vec<u8> = Vec::with_capacity(len as usize);
        buffer.set_len((len as usize) - 1);

        gl::GetProgramInfoLog(shader_program, len, ptr::null_mut(), buffer.as_mut_ptr() as *mut _);

        panic!("ERROR::SHADER::PROGRAM::LINKING_FAILED\n{}", std::str::from_utf8(&buffer).unwrap());
    }

    gl::DeleteShader(vertex_shader);
    gl::DeleteShader(fragment_shader);

    shader_program
}

fn get_rainbow(hue: f32) -> (f32, f32, f32) {
    let hsv = Hsv::new(hue, 1.0, 1.0);
    let rgb = Srgb::from_color(hsv);
    
    (rgb.red, rgb.green, rgb.blue)
}
