

vec4 composite1(sampler2D col1, sampler2D depth1) {

}




vec4 composite2(sampler2D col1, sampler2D depth1) {

}



void composite3(in sampler2D col1, in sampler2D depth1,
                in sampler2D col2, in sampler2D depth2,
                in sampler2D col3, in sampler2D depth3, vec2 uv) {

    float tex1_depth = texture(depth1, uv).r;
    float tex2_depth = texture(depth2, uv).r;
    float tex3_depth = texture(depth3, uv).r;
    
    vec4 tex1_color = texture(col1, uv);
    vec4 tex2_color = texture(col2, uv);
    vec4 tex3_color = texture(col3, uv);

    if(tex1_depth < tex2_depth && tex1_depth < tex3_depth) {
        gl_FragDepth = tex1_depth;
        fragColor = vec4(tex1_color.rgb, 1.0);
    }
    else if(tex2_depth < tex3_depth) {
        gl_FragDepth = tex2_depth;
        fragColor = vec4(tex2_color.rgb, 1.0);
    }
    else {
        gl_FragDepth = tex3_depth;
        fragColor = vec4(tex3_color.rgb, 1.0);
    }
}



vec4 composite4(sampler2D col1, sampler2D depth1) {

}



vec4 composite5(sampler2D col1, sampler2D depth1) {

}


vec4 composite6(sampler2D col1, sampler2D depth1) {

}


vec4 composite7(sampler2D col1, sampler2D depth1) {

}
