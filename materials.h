#ifndef MATERIAL_H
#define MATERIAL_H


typedef struct MaterialParams {
    float emissive[4];
    float ambient[4];
    float diffuse[4];
    float specular[4];
    float shininess;
} Material;

Material empty = 
    {
      {0, 0, 0, 0},  
      {0, 0, 0, 0}, 
      {0, 0, 0, 0},  
      {0, 0, 0, 0}, 
      0
    };

Material lightMat = 
    {
      {1, 1, 1, 1},  
      {1, 1, 1, 1}, 
      {1, 1, 1, 1},  
      {1, 1, 1, 1}, 
      1
    };
Material copper = 
    {
      {0,0,0,1}, 
      {0.19f, 0.07f, 0.02f, 1.0f}, 
      {0.70f, 0.27f, 0.08f, 1.0f}, 
      {0.26f, 0.14f, 0.09f, 1.0f},
      12.8f
    };
Material rubin = 
    {
        {0,0,0,1},
        {0.17f, 0.01f, 0.01f, 0.50f},
        {0.61f, 0.04f, 0.04f, 0.50f},
        {0.73f, 0.63f, 0.63f, 0.50f},
        76.8f
    };
Material gold = 
    {
        {0,0,0,1},
        {0.25f, 0.22f, 0.06f, 1.0f},
        {0.35f, 0.31f, 0.09f, 1.0f},
        {0.80f, 0.72f, 0.21f, 1.0f},
        83.2f
    };
Material chrom = 
    {
        {0,0,0,1},
        {0.25f, 0.25f, 0.25f, 1.0f},
        {0.40f, 0.40f, 0.40f, 1.0f},
        {0.77f, 0.77f, 0.77f, 1.0f},
        76.0f
    };

Material wood =
    {
        {0,0,0,1},
        {0.2f, 0.1f, 0.0f, 1.0f},
        {0.64f, 0.32f, 0.12f, 1.0f},
        {0.5f, 0.5f, 0.5f, 1.0f},
        10.0f
    };
#endif