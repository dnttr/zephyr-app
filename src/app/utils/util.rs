use palette::{FromColor, Hsv, Srgb};

pub(crate) fn get_rainbow(hue: f32) -> (f32, f32, f32) {
    let hsv = Hsv::new(hue, 1.0, 1.0);
    let rgb = Srgb::from_color(hsv);

    (rgb.red, rgb.green, rgb.blue)
}