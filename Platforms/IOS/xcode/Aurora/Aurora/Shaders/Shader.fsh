//
//  Shader.fsh
//  Aurora
//
//  Created by h3d on 2017/3/27.
//  Copyright © 2017年 h3d. All rights reserved.
//

varying lowp vec4 colorVarying;

void main()
{
    gl_FragColor = colorVarying;
}
