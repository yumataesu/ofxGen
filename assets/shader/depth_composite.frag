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

uniform sampler2D u_col0;
uniform sampler2D u_col1;
uniform sampler2D u_col2;
uniform sampler2D u_col3;
uniform sampler2D u_col4;
uniform sampler2D u_col5;
uniform sampler2D u_col6;
uniform sampler2D u_col7;
uniform sampler2D u_col8;

uniform sampler2D u_position0;
uniform sampler2D u_position1;
uniform sampler2D u_position2;
uniform sampler2D u_position3;
uniform sampler2D u_position4;
uniform sampler2D u_position5;
uniform sampler2D u_position6;
uniform sampler2D u_position7;
uniform sampler2D u_position8;

uniform float u_alpha0;
uniform float u_alpha1;
uniform float u_alpha2;
uniform float u_alpha3;
uniform float u_alpha4;
uniform float u_alpha5;
uniform float u_alpha6;
uniform float u_alpha7;
uniform float u_alpha8;

in vec2 v_texcoord;

layout(location = 0) out vec4 fragColor_color;
layout(location = 1) out vec4 fragColor_position;

void main() {

    if(u_layer_num == 0) {
        fragColor_color = vec4(0, 0, 0, 1);
        fragColor_position = vec4(0);
    }

    if(u_layer_num == 1) {
        float tex1_depth = texture(u_depth0, v_texcoord).r;
        vec4 tex1_color = texture(u_col0, v_texcoord);
        vec4 tex1_world = texture(u_position0, v_texcoord);

        gl_FragDepth = tex1_depth;
        fragColor_color = vec4(tex1_color.rgb * u_alpha0, u_alpha0);
        fragColor_position = vec4(tex1_world.xyz, 1.0);

    }
    else if(u_layer_num == 2) {
        float d0 = texture(u_depth0, v_texcoord).r;
        float d1 = texture(u_depth1, v_texcoord).r;
        
        vec4 c0 = texture(u_col0, v_texcoord);
        vec4 c1 = texture(u_col1, v_texcoord);

        vec4 p0 = texture(u_position0, v_texcoord);
        vec4 p1 = texture(u_position1, v_texcoord);
        
        if(d0 < d1) {
            gl_FragDepth = d0;
            fragColor_color = vec4(c0.rgb * u_alpha0, u_alpha0);
            fragColor_position = vec4(p0.rgb, u_alpha0);

            if(u_alpha0 < 0.001) { 
                fragColor_color = vec4(c1.rgb * u_alpha1, u_alpha1);
                fragColor_position = vec4(p1.rgb, 1.0); 
            }
        }
        else  {
            gl_FragDepth = d1;
            fragColor_color = vec4(c1.rgb * u_alpha1, u_alpha1);
            fragColor_position = vec4(p1.rgb, u_alpha1);

            if(u_alpha1 < 0.001) { 
                fragColor_color = vec4(c0.rgb * u_alpha0, u_alpha0);
                fragColor_position = vec4(p0.rgb, u_alpha0);
            }
        }
    }
    else if(u_layer_num == 3) {
        float d0 = texture(u_depth0, v_texcoord).r;
        float d1 = texture(u_depth1, v_texcoord).r;
        float d2 = texture(u_depth2, v_texcoord).r;
        vec4 c0  = texture(u_col0, v_texcoord);
        vec4 c1  = texture(u_col1, v_texcoord);
        vec4 c2  = texture(u_col2, v_texcoord);
        vec4 p0  = texture(u_position0, v_texcoord);
        vec4 p1  = texture(u_position1, v_texcoord);
        vec4 p2  = texture(u_position2, v_texcoord);

        if(u_alpha0 < 0.001) {
            if(d1 < d2) {
                gl_FragDepth = d1;
                fragColor_color = vec4(c1.rgb, u_alpha1);
                fragColor_position = vec4(p1.rgb, u_alpha1);

                if(u_alpha1 < 0.001) { 
                    fragColor_color = vec4(c2.rgb, u_alpha2); 
                    fragColor_position = vec4(p2.rgb, u_alpha2);
                }
            }
            else  {
                gl_FragDepth = d2;
                fragColor_color = vec4(c2.rgb, u_alpha2);
                fragColor_position = vec4(p2.rgb, u_alpha2);

                if(u_alpha2 < 0.001) { 
                    fragColor_color = vec4(c1.rgb, u_alpha1); 
                    fragColor_position = vec4(p1.rgb, u_alpha1); 
                }
            }
        } else if(u_alpha1 < 0.001) {
            if(d0 < d2) {
                gl_FragDepth = d0;
                fragColor_color = vec4(c0.rgb, u_alpha0);
                fragColor_position = vec4(p0.rgb, u_alpha0);

                if(u_alpha0 < 0.001) { 
                    fragColor_color = vec4(c2.rgb, u_alpha2); 
                    fragColor_position = vec4(p2.rgb, u_alpha2); 
                }
            }
            else  {
                gl_FragDepth = d2;
                fragColor_color = vec4(c2.rgb, u_alpha2);
                fragColor_position = vec4(p2.rgb, u_alpha2);

                if(u_alpha2 < 0.001) { 
                    fragColor_color = vec4(c0.rgb, u_alpha0); 
                    fragColor_position = vec4(p0.rgb, u_alpha0); 
                }
            }
        } else if(u_alpha2 < 0.001) {
            if(d0 < d1) {
                gl_FragDepth = d0;
                fragColor_color = vec4(c0.rgb, u_alpha0);
                fragColor_position = vec4(p0.rgb, u_alpha0);

                if(u_alpha0 < 0.001) { 
                    fragColor_color = vec4(c1.rgb, u_alpha1); 
                    fragColor_position = vec4(p1.rgb, u_alpha1); 
                }
            }
            else  {
                gl_FragDepth = d2;
                fragColor_color = vec4(c1.rgb, u_alpha1);
                fragColor_position = vec4(p1.rgb, u_alpha1); 

                if(u_alpha1 < 0.001) { 
                    fragColor_color = vec4(c0.rgb, u_alpha0); 
                    fragColor_position = vec4(p0.rgb, u_alpha0);
                }
            }
        } else {
            if(d0 < d1 && d0 < d2) {
                fragColor_color = vec4(c0.rgb, u_alpha0);
                fragColor_position = vec4(p0.rgb, u_alpha0);
            } else if(d1 < d2) {
                fragColor_color = vec4(c1.rgb, u_alpha1);
                fragColor_position = vec4(p1.rgb, u_alpha1);
            } else {
                fragColor_color = vec4(c2.rgb, u_alpha2);
                fragColor_position = vec4(p2.rgb, u_alpha2);
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

        vec4 p0  = texture(u_position0, v_texcoord);
        vec4 p1  = texture(u_position1, v_texcoord);
        vec4 p2  = texture(u_position2, v_texcoord);
        vec4 p3  = texture(u_position3, v_texcoord);

        if(u_alpha0 < 0.001) {
            if(u_alpha1 < 0.001) {
                if(d2 < d3) {
                    gl_FragDepth = d2;
                    fragColor_color = vec4(c2.rgb, u_alpha2);
                    fragColor_position = vec4(p2.rgb, u_alpha2);
                    if(u_alpha2 < 0.001) { 
                        fragColor_color = vec4(c3.rgb, u_alpha3); 
                        fragColor_position = vec4(p3.rgb, u_alpha3);
                    }
                }
                else  {
                    gl_FragDepth = d3;
                    fragColor_color = vec4(c3.rgb, u_alpha3);
                    fragColor_position = vec4(p3.rgb, u_alpha3);

                    if(u_alpha3 < 0.001) { 
                        fragColor_color = vec4(c2.rgb, u_alpha2); 
                        fragColor_position = vec4(p2.rgb, u_alpha2); 
                    }
                }
            } 
            else if(u_alpha2 < 0.001) {
                if(d1 < d3) {
                    gl_FragDepth = d1;
                    fragColor_color = vec4(c1.rgb, u_alpha1);
                    fragColor_position = vec4(p1.rgb, u_alpha1);

                    if(u_alpha1 < 0.001) { 
                        fragColor_color = vec4(c3.rgb, u_alpha3); 
                        fragColor_position = vec4(p3.rgb, u_alpha3); 
                    }
                }
                else  {
                    gl_FragDepth = d3;
                    fragColor_color = vec4(c3.rgb, u_alpha3);
                    fragColor_position = vec4(p3.rgb, u_alpha3);

                    if(u_alpha3 < 0.001) { 
                        fragColor_color = vec4(c1.rgb, u_alpha1);
                        fragColor_position = vec4(p1.rgb, u_alpha1);
                    }
                }
            } else if(u_alpha3 < 0.001) {
                if(d1 < d2) {
                    gl_FragDepth = d1;
                    fragColor_color = vec4(c1.rgb, u_alpha1);
                    fragColor_position = vec4(p1.rgb, u_alpha1);

                    if(u_alpha2 < 0.001) { 
                        fragColor_color = vec4(c2.rgb, u_alpha2);
                        fragColor_position = vec4(p2.rgb, u_alpha2);
                    }
                }
                else  {
                    gl_FragDepth = d2;
                    fragColor_color = vec4(c2.rgb, u_alpha2);
                    fragColor_position = vec4(p2.rgb, u_alpha2);

                    if(u_alpha2 < 0.001) { 
                        fragColor_color = vec4(c1.rgb, u_alpha1); 
                        fragColor_position = vec4(p1.rgb, u_alpha1); 
                    }
                }
            } else {
                if(d1 < d2 && d1 < d3) {
                    fragColor_color = vec4(c1.rgb, u_alpha1);
                    fragColor_position = vec4(p1.rgb, u_alpha1);
                } else if(d2 < d3) {
                    fragColor_color = vec4(c2.rgb, u_alpha2);
                    fragColor_position = vec4(p2.rgb, u_alpha2);
                } else {
                    fragColor_color = vec4(c3.rgb, u_alpha3);
                    fragColor_position = vec4(p3.rgb, u_alpha3);
                }
            }
        } else if(u_alpha1 < 0.001) {
            if(u_alpha0 < 0.001) {
                if(d2 < d3) {
                    gl_FragDepth = d2;
                    fragColor_color = vec4(c2.rgb, u_alpha2);
                    fragColor_position = vec4(p2.rgb, u_alpha2);

                    if(u_alpha2 < 0.001) { 
                        fragColor_color = vec4(c3.rgb, u_alpha3); 
                        fragColor_position = vec4(p3.rgb, u_alpha3);
                    }
                }
                else  {
                    gl_FragDepth = d3;
                    fragColor_color = vec4(c3.rgb, u_alpha3);
                    fragColor_position = vec4(p3.rgb, u_alpha3);

                    if(u_alpha3 < 0.001) { 
                        fragColor_color = vec4(c2.rgb, u_alpha2); 
                        fragColor_position = vec4(p2.rgb, u_alpha2);
                    }
                }
            }
            else if(u_alpha2 < 0.001) {
                if(d0 < d3) {
                    gl_FragDepth = d0;
                    fragColor_color = vec4(c0.rgb, u_alpha0);
                    fragColor_position = vec4(p0.rgb, u_alpha0);

                    if(u_alpha0 < 0.001) { 
                        fragColor_color = vec4(c3.rgb, u_alpha3); 
                        fragColor_position = vec4(p3.rgb, u_alpha3);
                    }
                }
                else  {
                    gl_FragDepth = d3;
                    fragColor_color = vec4(c3.rgb, u_alpha3);
                    fragColor_position = vec4(p3.rgb, u_alpha3);

                    if(u_alpha0 < 0.001) { 
                        fragColor_color = vec4(c0.rgb, u_alpha0); 
                        fragColor_position = vec4(p0.rgb, u_alpha0);
                    }
                }
            } else if(u_alpha3 < 0.001) {
                if(d0 < d2) {
                    gl_FragDepth = d0;
                    fragColor_color = vec4(c0.rgb, u_alpha0);
                    fragColor_position = vec4(p0.rgb, u_alpha0);
                
                    if(u_alpha0 < 0.001) { 
                        fragColor_color = vec4(c2.rgb, u_alpha2); 
                        fragColor_position = vec4(p2.rgb, u_alpha2);                
                    }
                }
                else  {
                    gl_FragDepth = d2;
                    fragColor_color = vec4(c2.rgb, u_alpha2);
                    fragColor_position = vec4(p2.rgb, u_alpha2);                
                
                    if(u_alpha2 < 0.001) { 
                        fragColor_color = vec4(c0.rgb, u_alpha0); 
                        fragColor_position = vec4(p0.rgb, u_alpha0);
                    }
                }
            } else {
                if(d0 < d2 && d0 < d3) {
                    fragColor_color = vec4(c0.rgb, u_alpha0);
                    fragColor_position = vec4(p0.rgb, u_alpha0);
                } else if(d2 < d3) {
                    fragColor_color = vec4(c2.rgb, u_alpha2);
                    fragColor_position = vec4(p2.rgb, u_alpha2);
                } else {
                    fragColor_color = vec4(c3.rgb, u_alpha3);
                    fragColor_position = vec4(p3.rgb, u_alpha3);
                }
            }
        } 
        else if(u_alpha2 < 0.001) {
            if(u_alpha0 < 0.001) {
                if(d1 < d3) {
                    gl_FragDepth = d1;
                    fragColor_color = vec4(c1.rgb, u_alpha1);
                    fragColor_position = vec4(p1.rgb, u_alpha1);
                    if(u_alpha1 < 0.001) { 
                        fragColor_color = vec4(c3.rgb, u_alpha3); 
                        fragColor_position = vec4(p3.rgb, u_alpha3);
                    }
                }
                else  {
                    gl_FragDepth = d3;
                    fragColor_color = vec4(c3.rgb, u_alpha3);
                    if(u_alpha3 < 0.001) { 
                        fragColor_color = vec4(c1.rgb, u_alpha1); 
                        fragColor_position = vec4(p1.rgb, u_alpha1);
                    }
                }
            }
            else if(u_alpha1 < 0.001) {
                if(d0 < d3) {
                    gl_FragDepth = d0;
                    fragColor_color = vec4(c0.rgb, u_alpha0);
                    fragColor_position = vec4(p0.rgb, u_alpha0);
                    if(u_alpha0 < 0.001) { 
                        fragColor_color = vec4(c3.rgb, u_alpha3); 
                        fragColor_position = vec4(p3.rgb, u_alpha3);
                    }
                }
                else  {
                    gl_FragDepth = d3;
                    fragColor_color = vec4(c3.rgb, u_alpha3);
                    fragColor_position = vec4(p3.rgb, u_alpha3);
                    if(u_alpha0 < 0.001) { 
                        fragColor_color = vec4(c0.rgb, u_alpha0); 
                        fragColor_position = vec4(p0.rgb, u_alpha0);
                    }
                }
            } else if(u_alpha3 < 0.001) {
                if(d0 < d1) {
                    gl_FragDepth = d0;
                    fragColor_color = vec4(c0.rgb, u_alpha0);
                    fragColor_position = vec4(p0.rgb, u_alpha0);
                    if(u_alpha0 < 0.001) { 
                        fragColor_color = vec4(c1.rgb, u_alpha1); 
                        fragColor_position = vec4(p1.rgb, u_alpha1);
                    }
                }
                else  {
                    gl_FragDepth = d1;
                    fragColor_color = vec4(c1.rgb, u_alpha1);
                    fragColor_position = vec4(p1.rgb, u_alpha1);
                    if(u_alpha1 < 0.001) { 
                        fragColor_color = vec4(c0.rgb, u_alpha0); 
                        fragColor_position = vec4(p0.rgb, u_alpha0);
                    }
                }
            } else {
                if(d0 < d1 && d0 < d3) {
                    fragColor_color = vec4(c0.rgb, u_alpha0);
                    fragColor_position = vec4(p0.rgb, u_alpha0);
                } else if(d1 < d3) {
                    fragColor_color = vec4(c1.rgb, u_alpha1);
                    fragColor_position = vec4(p1.rgb, u_alpha1);
                } else {
                    fragColor_color = vec4(c3.rgb, u_alpha3);
                    fragColor_position = vec4(p3.rgb, u_alpha3);
                }
            }
        } 
        else if(u_alpha3 < 0.001) {
            if(u_alpha0 < 0.001) {
                if(d1 < d2) {
                    gl_FragDepth = d1;
                    fragColor_color = vec4(c1.rgb, u_alpha1);
                    fragColor_position = vec4(p1.rgb, u_alpha1);
                    if(u_alpha1 < 0.001) { 
                        fragColor_color = vec4(c2.rgb, u_alpha2); 
                        fragColor_position = vec4(p2.rgb, u_alpha2);
                    }
                }
                else  {
                    gl_FragDepth = d2;
                    fragColor_color = vec4(c2.rgb, u_alpha2);
                    fragColor_position = vec4(p2.rgb, u_alpha2);
                    if(u_alpha2 < 0.001) { 
                        fragColor_color = vec4(c1.rgb, u_alpha1); 
                        fragColor_position = vec4(p1.rgb, u_alpha1);
                    }
                }
            }
            else if(u_alpha1 < 0.001) {
                if(d0 < d2) {
                    gl_FragDepth = d0;
                    fragColor_color = vec4(c0.rgb, u_alpha0);
                    fragColor_position = vec4(p0.rgb, u_alpha0);
                    if(u_alpha0 < 0.001) { 
                        fragColor_color = vec4(c2.rgb, u_alpha2); 
                        fragColor_position = vec4(p2.rgb, u_alpha2);
                    }
                }
                else  {
                    gl_FragDepth = d2;
                    fragColor_color = vec4(c2.rgb, u_alpha2);
                    fragColor_position = vec4(p2.rgb, u_alpha2);
                    if(u_alpha2 < 0.001) { 
                        fragColor_color = vec4(c0.rgb, u_alpha0); 
                        fragColor_position = vec4(p0.rgb, u_alpha0);
                    }
                }
            } else if(u_alpha2 < 0.001) {
                if(d0 < d1) {
                    gl_FragDepth = d0;
                    fragColor_color = vec4(c0.rgb, u_alpha0);
                    fragColor_position = vec4(p0.rgb, u_alpha0);
                    if(u_alpha0 < 0.001) { 
                        fragColor_color = vec4(c1.rgb, u_alpha1); 
                        fragColor_position = vec4(p1.rgb, u_alpha1);
                    }
                }
                else  {
                    gl_FragDepth = d1;
                    fragColor_color = vec4(c1.rgb, u_alpha1);
                    fragColor_position = vec4(p1.rgb, u_alpha1);
                    if(u_alpha1 < 0.001) { 
                        fragColor_color = vec4(c0.rgb, u_alpha0); 
                        fragColor_position = vec4(c0.rgb, u_alpha0); 
                    }
                }
            } else {
                if(d0 < d1 && d0 < d2) {
                    fragColor_color = vec4(c0.rgb, u_alpha0);
                    fragColor_position = vec4(c0.rgb, u_alpha0); 

                } else if(d1 < d3) {
                    fragColor_color = vec4(c1.rgb, u_alpha1);
                    fragColor_position = vec4(c1.rgb, u_alpha1); 
                } else {
                    fragColor_color = vec4(c2.rgb, u_alpha2);
                    fragColor_position = vec4(c2.rgb, u_alpha2); 
                }
            }
        } else {
            if(d0 < d1 && d0 < d2 && d0 < d3) {
                fragColor_color = vec4(c0.rgb, u_alpha0);
                fragColor_position = vec4(c0.rgb, u_alpha0); 
            } else if(d1 < d2 && d1 < d3) {
                fragColor_color = vec4(c1.rgb, u_alpha1);
                fragColor_position = vec4(c1.rgb, u_alpha1); 
            } else if(d2 < d3) {
                fragColor_color = vec4(c2.rgb, u_alpha2);
                fragColor_position = vec4(c2.rgb, u_alpha2); 
            } else {
                fragColor_color = vec4(c3.rgb, u_alpha3);
                fragColor_position = vec4(c3.rgb, u_alpha3); 
            }
        }
    }
    //still
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
            fragColor_color = vec4(tex1_color.rgb, u_alpha0);
        }
        else if(tex2_depth < tex3_depth && tex2_depth < tex4_depth && tex2_depth < tex5_depth) {
            gl_FragDepth = tex2_depth;
            fragColor_color = vec4(tex2_color.rgb, u_alpha1);
        }
        else if(tex3_depth < tex4_depth && tex3_depth < tex5_depth) {
            gl_FragDepth = tex3_depth;
            fragColor_color = vec4(tex3_color.rgb, u_alpha2);
        }
        else if(tex4_depth < tex5_depth) {
            gl_FragDepth = tex4_depth;
            fragColor_color = vec4(tex4_color.rgb, u_alpha3);
        }
        else {
            gl_FragDepth = tex5_depth;
            fragColor_color = vec4(tex5_color.rgb, u_alpha4);
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
            fragColor_color = vec4(tex1_color.rgb, u_alpha0);
        }
        else if(tex2_depth < tex3_depth && tex2_depth < tex4_depth && tex2_depth < tex5_depth && tex2_depth < tex6_depth) {
            gl_FragDepth = tex2_depth;
            fragColor_color = vec4(tex2_color.rgb, u_alpha1);
        }
        else if(tex3_depth < tex4_depth && tex3_depth < tex5_depth && tex3_depth < tex6_depth) {
            gl_FragDepth = tex3_depth;
            fragColor_color = vec4(tex3_color.rgb, u_alpha2);
        }
        else if(tex4_depth < tex5_depth && tex4_depth < tex6_depth) {
            gl_FragDepth = tex4_depth;
            fragColor_color = vec4(tex4_color.rgb, u_alpha3);
        }
        else if(tex5_depth < tex6_depth) {
            gl_FragDepth = tex5_depth;
            fragColor_color = vec4(tex5_color.rgb, u_alpha4);
        }
        else {
            gl_FragDepth = tex6_depth;
            fragColor_color = vec4(tex6_color.rgb, u_alpha5);
        }
    }
}
