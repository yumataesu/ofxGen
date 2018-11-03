#version 410

uniform int u_layer_num;

uniform sampler2D u_depth0;
uniform sampler2D u_depth1;
uniform sampler2D u_depth2;
uniform sampler2D u_depth3;
uniform sampler2D u_depth4;
uniform sampler2D u_depth5;
uniform sampler2D u_depth6;
uniform sampler2D u_depth7;
uniform sampler2D u_depth8;
uniform sampler2D u_depth9;
uniform sampler2D u_depth10;
uniform sampler2D u_depth11;
uniform sampler2D u_depth12;

uniform sampler2D u_col0;
uniform sampler2D u_col1;
uniform sampler2D u_col2;
uniform sampler2D u_col3;
uniform sampler2D u_col4;
uniform sampler2D u_col5;
uniform sampler2D u_col6;
uniform sampler2D u_col7;
uniform sampler2D u_col8;
uniform sampler2D u_col9;
uniform sampler2D u_col10;
uniform sampler2D u_col11;
uniform sampler2D u_col12;

uniform float u_alpha0;
uniform float u_alpha1;
uniform float u_alpha2;
uniform float u_alpha3;
uniform float u_alpha4;
uniform float u_alpha5;
uniform float u_alpha6;
uniform float u_alpha7;
uniform float u_alpha8;
uniform float u_alpha9;
uniform float u_alpha10;
uniform float u_alpha11;
uniform float u_alpha12;

in vec2 v_texcoord;

out vec4 fragColor;

void main() {

    if(u_layer_num == 1) {
        float tex1_depth = texture(u_depth0, v_texcoord).r;
        vec4 tex1_color = texture(u_col0, v_texcoord);

        gl_FragDepth = tex1_depth;
        fragColor = vec4(tex1_color.rgb, u_alpha0);
    }
    else if(u_layer_num == 2) {
        float d0 = texture(u_depth0, v_texcoord).r;
        float d1 = texture(u_depth1, v_texcoord).r;
        
        vec4 c0 = texture(u_col0, v_texcoord);
        vec4 c1 = texture(u_col1, v_texcoord);
        
        if(d0 < d1) {
            gl_FragDepth = d0;
            fragColor = vec4(c0.rgb, u_alpha0);
            if(u_alpha0 < 0.001) { fragColor = vec4(c1.rgb, u_alpha1); }
        }
        else  {
            gl_FragDepth = d1;
            fragColor = vec4(c1.rgb, u_alpha1);
            if(u_alpha1 < 0.001) { fragColor = vec4(c0.rgb, u_alpha0); }
        }
    }
    else if(u_layer_num == 3) {
        float d0 = texture(u_depth0, v_texcoord).r;
        float d1 = texture(u_depth1, v_texcoord).r;
        float d2 = texture(u_depth2, v_texcoord).r;
        vec4 c0  = texture(u_col0, v_texcoord);
        vec4 c1  = texture(u_col1, v_texcoord);
        vec4 c2  = texture(u_col2, v_texcoord);

        if(u_alpha0 < 0.001) {
            if(d1 < d2) {
                gl_FragDepth = d1;
                fragColor = vec4(c1.rgb, u_alpha1);
                if(u_alpha1 < 0.001) { fragColor = vec4(c2.rgb, u_alpha2); }
            }
            else  {
                gl_FragDepth = d2;
                fragColor = vec4(c2.rgb, u_alpha2);
                if(u_alpha2 < 0.001) { fragColor = vec4(c1.rgb, u_alpha1); }
            }
        } else if(u_alpha1 < 0.001) {
            if(d0 < d2) {
                gl_FragDepth = d0;
                fragColor = vec4(c0.rgb, u_alpha0);
                if(u_alpha0 < 0.001) { fragColor = vec4(c2.rgb, u_alpha2); }
            }
            else  {
                gl_FragDepth = d2;
                fragColor = vec4(c2.rgb, u_alpha2);
                if(u_alpha2 < 0.001) { fragColor = vec4(c0.rgb, u_alpha0); }
            }
        } else if(u_alpha2 < 0.001) {
            if(d0 < d1) {
                gl_FragDepth = d0;
                fragColor = vec4(c0.rgb, u_alpha0);
                if(u_alpha0 < 0.001) { fragColor = vec4(c1.rgb, u_alpha1); }
            }
            else  {
                gl_FragDepth = d2;
                fragColor = vec4(c1.rgb, u_alpha1);
                if(u_alpha1 < 0.001) { fragColor = vec4(c0.rgb, u_alpha0); }
            }
        } else {
            if(d0 < d1 && d0 < d2) {
                fragColor = vec4(c0.rgb, u_alpha0);
            } else if(d1 < d2) {
                fragColor = vec4(c1.rgb, u_alpha1);
            } else {
                fragColor = vec4(c2.rgb, u_alpha2);
            }
        }
    }

    else if(u_layer_num == 4) {
        float d0 = texture(u_depth0, v_texcoord).r;
        float d1 = texture(u_depth1, v_texcoord).r;
        float d2 = texture(u_depth2, v_texcoord).r;
        float d3 = texture(u_depth3, v_texcoord).r;

        vec4 c0  = texture(u_col0, v_texcoord);
        vec4 c1  = texture(u_col1, v_texcoord);
        vec4 c2  = texture(u_col2, v_texcoord);
        vec4 c3  = texture(u_col3, v_texcoord);

        if(u_alpha0 < 0.001) {
            if(u_alpha1 < 0.001) {
                if(d2 < d3) {
                    gl_FragDepth = d2;
                    fragColor = vec4(c2.rgb, u_alpha2);
                    if(u_alpha2 < 0.001) { fragColor = vec4(c3.rgb, u_alpha3); }
                }
                else  {
                    gl_FragDepth = d3;
                    fragColor = vec4(c3.rgb, u_alpha3);
                    if(u_alpha3 < 0.001) { fragColor = vec4(c2.rgb, u_alpha2); }
                }
            } 
            else if(u_alpha2 < 0.001) {
                if(d1 < d3) {
                    gl_FragDepth = d1;
                    fragColor = vec4(c1.rgb, u_alpha1);
                    if(u_alpha1 < 0.001) { fragColor = vec4(c3.rgb, u_alpha3); }
                }
                else  {
                    gl_FragDepth = d3;
                    fragColor = vec4(c3.rgb, u_alpha3);
                    if(u_alpha3 < 0.001) { fragColor = vec4(c1.rgb, u_alpha1); }
                }
            } else if(u_alpha3 < 0.001) {
                if(d1 < d2) {
                    gl_FragDepth = d1;
                    fragColor = vec4(c1.rgb, u_alpha1);
                    if(u_alpha2 < 0.001) { fragColor = vec4(c2.rgb, u_alpha2); }
                }
                else  {
                    gl_FragDepth = d2;
                    fragColor = vec4(c2.rgb, u_alpha2);
                    if(u_alpha2 < 0.001) { fragColor = vec4(c1.rgb, u_alpha1); }
                }
            } else {
                if(d1 < d2 && d1 < d3) {
                    fragColor = vec4(c1.rgb, u_alpha1);
                } else if(d2 < d3) {
                    fragColor = vec4(c2.rgb, u_alpha2);
                } else {
                    fragColor = vec4(c3.rgb, u_alpha3);
                }
            }
        } else if(u_alpha1 < 0.001) {
            if(u_alpha0 < 0.001) {
                if(d2 < d3) {
                    gl_FragDepth = d2;
                    fragColor = vec4(c2.rgb, u_alpha2);
                    if(u_alpha2 < 0.001) { fragColor = vec4(c3.rgb, u_alpha3); }
                }
                else  {
                    gl_FragDepth = d3;
                    fragColor = vec4(c3.rgb, u_alpha3);
                    if(u_alpha3 < 0.001) { fragColor = vec4(c2.rgb, u_alpha2); }
                }
            }
            else if(u_alpha2 < 0.001) {
                if(d0 < d3) {
                    gl_FragDepth = d0;
                    fragColor = vec4(c0.rgb, u_alpha0);
                    if(u_alpha0 < 0.001) { fragColor = vec4(c3.rgb, u_alpha3); }
                }
                else  {
                    gl_FragDepth = d3;
                    fragColor = vec4(c3.rgb, u_alpha3);
                    if(u_alpha0 < 0.001) { fragColor = vec4(c0.rgb, u_alpha0); }
                }
            } else if(u_alpha3 < 0.001) {
                if(d0 < d2) {
                    gl_FragDepth = d0;
                    fragColor = vec4(c0.rgb, u_alpha0);
                    if(u_alpha0 < 0.001) { fragColor = vec4(c2.rgb, u_alpha2); }
                }
                else  {
                    gl_FragDepth = d2;
                    fragColor = vec4(c2.rgb, u_alpha2);
                    if(u_alpha2 < 0.001) { fragColor = vec4(c0.rgb, u_alpha0); }
                }
            } else {
                if(d0 < d2 && d0 < d3) {
                    fragColor = vec4(c0.rgb, u_alpha0);
                } else if(d2 < d3) {
                    fragColor = vec4(c2.rgb, u_alpha2);
                } else {
                    fragColor = vec4(c3.rgb, u_alpha3);
                }
            }
        } 
        else if(u_alpha2 < 0.001) {
            if(u_alpha0 < 0.001) {
                if(d1 < d3) {
                    gl_FragDepth = d1;
                    fragColor = vec4(c1.rgb, u_alpha1);
                    if(u_alpha1 < 0.001) { fragColor = vec4(c3.rgb, u_alpha3); }
                }
                else  {
                    gl_FragDepth = d3;
                    fragColor = vec4(c3.rgb, u_alpha3);
                    if(u_alpha3 < 0.001) { fragColor = vec4(c1.rgb, u_alpha1); }
                }
            }
            else if(u_alpha1 < 0.001) {
                if(d0 < d3) {
                    gl_FragDepth = d0;
                    fragColor = vec4(c0.rgb, u_alpha0);
                    if(u_alpha0 < 0.001) { fragColor = vec4(c3.rgb, u_alpha3); }
                }
                else  {
                    gl_FragDepth = d3;
                    fragColor = vec4(c3.rgb, u_alpha3);
                    if(u_alpha0 < 0.001) { fragColor = vec4(c0.rgb, u_alpha0); }
                }
            } else if(u_alpha3 < 0.001) {
                if(d0 < d1) {
                    gl_FragDepth = d0;
                    fragColor = vec4(c0.rgb, u_alpha0);
                    if(u_alpha0 < 0.001) { fragColor = vec4(c1.rgb, u_alpha1); }
                }
                else  {
                    gl_FragDepth = d1;
                    fragColor = vec4(c1.rgb, u_alpha1);
                    if(u_alpha1 < 0.001) { fragColor = vec4(c0.rgb, u_alpha0); }
                }
            } else {
                if(d0 < d1 && d0 < d3) {
                    fragColor = vec4(c0.rgb, u_alpha0);
                } else if(d1 < d3) {
                    fragColor = vec4(c1.rgb, u_alpha1);
                } else {
                    fragColor = vec4(c3.rgb, u_alpha3);
                }
            }
        } 
        else if(u_alpha3 < 0.001) {
            if(u_alpha0 < 0.001) {
                if(d1 < d2) {
                    gl_FragDepth = d1;
                    fragColor = vec4(c1.rgb, u_alpha1);
                    if(u_alpha1 < 0.001) { fragColor = vec4(c2.rgb, u_alpha2); }
                }
                else  {
                    gl_FragDepth = d2;
                    fragColor = vec4(c2.rgb, u_alpha2);
                    if(u_alpha2 < 0.001) { fragColor = vec4(c1.rgb, u_alpha1); }
                }
            }
            else if(u_alpha1 < 0.001) {
                if(d0 < d2) {
                    gl_FragDepth = d0;
                    fragColor = vec4(c0.rgb, u_alpha0);
                    if(u_alpha0 < 0.001) { fragColor = vec4(c2.rgb, u_alpha2); }
                }
                else  {
                    gl_FragDepth = d2;
                    fragColor = vec4(c2.rgb, u_alpha2);
                    if(u_alpha2 < 0.001) { fragColor = vec4(c0.rgb, u_alpha0); }
                }
            } else if(u_alpha2 < 0.001) {
                if(d0 < d1) {
                    gl_FragDepth = d0;
                    fragColor = vec4(c0.rgb, u_alpha0);
                    if(u_alpha0 < 0.001) { fragColor = vec4(c1.rgb, u_alpha1); }
                }
                else  {
                    gl_FragDepth = d1;
                    fragColor = vec4(c1.rgb, u_alpha1);
                    if(u_alpha1 < 0.001) { fragColor = vec4(c0.rgb, u_alpha0); }
                }
            } else {
                if(d0 < d1 && d0 < d2) {
                    fragColor = vec4(c0.rgb, u_alpha0);
                } else if(d1 < d3) {
                    fragColor = vec4(c1.rgb, u_alpha1);
                } else {
                    fragColor = vec4(c2.rgb, u_alpha2);
                }
            }
        } else {
            if(d0 < d1 && d0 < d2 && d0 < d3) {
                fragColor = vec4(c0.rgb, u_alpha0);
            } else if(d1 < d2 && d1 < d3) {
                fragColor = vec4(c1.rgb, u_alpha1);
            } else if(d2 < d3) {
                fragColor = vec4(c2.rgb, u_alpha2);
            } else {
                fragColor = vec4(c3.rgb, u_alpha3);
            }
        }
    }
    else if(u_layer_num == 5) {
        float tex1_depth = texture(u_depth0, v_texcoord).r;
        float tex2_depth = texture(u_depth1, v_texcoord).r;
        float tex3_depth = texture(u_depth2, v_texcoord).r;
        float tex4_depth = texture(u_depth3, v_texcoord).r;
        float tex5_depth = texture(u_depth4, v_texcoord).r;
        
        vec4 tex1_color = texture(u_col0, v_texcoord);
        vec4 tex2_color = texture(u_col1, v_texcoord);
        vec4 tex3_color = texture(u_col2, v_texcoord);
        vec4 tex4_color = texture(u_col3, v_texcoord);
        vec4 tex5_color = texture(u_col4, v_texcoord);
        
        if(tex1_depth < tex2_depth && tex1_depth < tex3_depth && tex1_depth < tex4_depth && tex1_depth < tex5_depth) {
            gl_FragDepth = tex1_depth;
            fragColor = vec4(tex1_color.rgb, u_alpha0);
        }
        else if(tex2_depth < tex3_depth && tex2_depth < tex4_depth && tex2_depth < tex5_depth) {
            gl_FragDepth = tex2_depth;
            fragColor = vec4(tex2_color.rgb, u_alpha1);
        }
        else if(tex3_depth < tex4_depth && tex3_depth < tex5_depth) {
            gl_FragDepth = tex3_depth;
            fragColor = vec4(tex3_color.rgb, u_alpha2);
        }
        else if(tex4_depth < tex5_depth) {
            gl_FragDepth = tex4_depth;
            fragColor = vec4(tex4_color.rgb, u_alpha3);
        }
        else {
            gl_FragDepth = tex5_depth;
            fragColor = vec4(tex5_color.rgb, u_alpha4);
        }
    }
    else if(u_layer_num == 6) {
        float tex1_depth = texture(u_depth0, v_texcoord).r;
        float tex2_depth = texture(u_depth1, v_texcoord).r;
        float tex3_depth = texture(u_depth2, v_texcoord).r;
        float tex4_depth = texture(u_depth3, v_texcoord).r;
        float tex5_depth = texture(u_depth4, v_texcoord).r;
        float tex6_depth = texture(u_depth5, v_texcoord).r;

        vec4 tex1_color = texture(u_col0, v_texcoord);
        vec4 tex2_color = texture(u_col1, v_texcoord);
        vec4 tex3_color = texture(u_col2, v_texcoord);
        vec4 tex4_color = texture(u_col3, v_texcoord);
        vec4 tex5_color = texture(u_col4, v_texcoord);
        vec4 tex6_color = texture(u_col5, v_texcoord);

        if(tex1_depth < tex2_depth && tex1_depth < tex3_depth && tex1_depth < tex4_depth && tex1_depth < tex5_depth && tex1_depth < tex6_depth) {
            gl_FragDepth = tex1_depth;
            fragColor = vec4(tex1_color.rgb, u_alpha0);
        }
        else if(tex2_depth < tex3_depth && tex2_depth < tex4_depth && tex2_depth < tex5_depth && tex2_depth < tex6_depth) {
            gl_FragDepth = tex2_depth;
            fragColor = vec4(tex2_color.rgb, u_alpha1);
        }
        else if(tex3_depth < tex4_depth && tex3_depth < tex5_depth && tex3_depth < tex6_depth) {
            gl_FragDepth = tex3_depth;
            fragColor = vec4(tex3_color.rgb, u_alpha2);
        }
        else if(tex4_depth < tex5_depth && tex4_depth < tex6_depth) {
            gl_FragDepth = tex4_depth;
            fragColor = vec4(tex4_color.rgb, u_alpha3);
        }
        else if(tex5_depth < tex6_depth) {
            gl_FragDepth = tex5_depth;
            fragColor = vec4(tex5_color.rgb, u_alpha4);
        }
        else {
            gl_FragDepth = tex6_depth;
            fragColor = vec4(tex6_color.rgb, u_alpha5);
        }
    }
}
