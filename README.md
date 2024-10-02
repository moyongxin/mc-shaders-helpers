# mc-shaders-helpers
Some helper scripts or programs for shaders development.


## Template .lang File Generator

[template_lang.py](scripts/template_lang.py)

Generate template.lang file from shaders.properties.

Currently only support generating profile, screen and option entries.

## Text Encodings Generator

[text_enc_gen](scripts/text_enc_gen/)

Generate 8px font encodings in glsl uvec2s.

Currently no layout information generated.

Usage:
```
python text_enc_gen.py <font> [-c charset_file] [-o output_glsl] [-t text]
```
where `output_glsl` is the output glsl definitions file for `charset_file` and emit a uvec2 glsl array to stdout for `text`.

