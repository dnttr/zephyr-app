use std::ffi::CString;
use std::ptr;

const VERTEX_SHADER_SOURCE: &str = r#"
#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 projection_matrix;

void main() {
    gl_Position = projection_matrix * vec4(aPos, 1.0);
}
"#;

const FRAGMENT_SHADER_SOURCE: &str = r#"
#version 330 core

uniform vec3 color;

out vec4 FragColor;
void main() {
    FragColor = vec4(color, 1.0);
}
"#;

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

pub unsafe fn create_shaders() -> u32 {
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
