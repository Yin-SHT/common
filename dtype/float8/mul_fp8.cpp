#include  "mul_fp8.h"

void mul_fp8_143_fp8_143_to_fp24(const uint8_t *x, const uint8_t *y, float& z, fp8_format nv_or_gr)
{
    uint8_t sign_x = 0x1 & (x[0]>>7);
    uint8_t exp_x = 0x0F & (x[0] >> 3);
    uint8_t val_x = 0x07 & x[0];

    uint8_t sign_y = 0x1 & (y[0]>>7);
    uint8_t exp_y = 0x0F & (y[0] >> 3);
    uint8_t val_y = 0x07 & y[0];
    // float z_32_x;
    // float z_32_y;
     float z_mul;
    // float z_32_x_max;
    // float z_32_y_max;
    // float z_max;
    uint32_t z_32_x;
    uint32_t z_32_y;
    // uint32_t z_mul;
    uint32_t z_32_x_max;
    uint32_t z_32_y_max;
    //uint32_t z_max;
	
    if(nv_or_gr==nv) //
    {
        if(((exp_x == 0xf) && (val_x == 0x7)) || ((exp_y == 0xf) && (val_y == 0x7)))//nan
        {
            self_float ret;
            ret.value = 0x7fc00000;
            z= ret.fp32;
           // z = 0x7fc00000; 
        }
        else{
             _fp8_143_to_float(x, nv, 7, (uint32_t*)(&z_32_x));//nv 43 
             _fp8_143_to_float(y, nv, 7,(uint32_t*)(&z_32_y));
             //z_mul = z_32_x * z_32_y;
             self_float z_32_x1;
             self_float z_32_y1;
             z_32_x1.value = z_32_x;
             z_32_y1.value = z_32_y;
              // printf("z_32_x1 =  %f\n", z_32_x1.fp32);
              // printf("z_32_y1 =  %f\n", z_32_y1.fp32);
            //  z_mul=subnormal(subnormal(z_32_x) * subnormal(z_32_y));
             z_mul=subnormal(subnormal(z_32_x1.fp32) * subnormal(z_32_y1.fp32));
            //  printf("z_mul =  %f\n", z_mul);
            // std::cout<<(int)z_mul<<endl;
             uint8_t x_max= sign_x<<7| 0x7e; //max_normal
             uint8_t y_max = sign_y<<7 | 0x7e;
             
             _fp8_143_to_float(&x_max, nv, 7, (uint32_t*)(&z_32_x_max));//nv 43 
             _fp8_143_to_float(&y_max, nv, 7,(uint32_t*)(&z_32_y_max));
             //self_float z_32_x_max1;
             //self_float z_32_y_max1;
             //z_32_x_max1.value = z_32_x_max;
             //z_32_y_max1.value = z_32_y_max;
             //z_max = z_32_x_max1.fp32 *  z_32_y_max1.fp32;
             /*if((sign_x ^ sign_y) ==1)//overflowuint
             {
                if(((int&)z_mul - 0x80000000) > ((int&)z_max))
                  {
                    z_mul=z_max;
                  } 
             }
             else{
                if(((int&)z_mul)  > ((int&)z_max ))
                 {
                    z_mul = z_max;
                 }
             }*/
              //_float_to_bf24(z_mul, (uint8_t*)(&z.value));
              if(std::isinf(z_mul))
              {
                z = (z_mul > 0) ? 0x7f7fff : 0xff7fff;
              }

               z=subnormal(f32_to_bf24(z_mul));
             //lhs.fp32 = f32_to_bf24(z_mul);
            //  if((int&)z > (int&)z_max)//round
            //  {
            //     z = z_max;

            //  }
            //  if((sign_x ^ sign_y)==1)//overflow
            //  {
            //     if(((int&)z - (int&)0x80000000) > ((int&)z_max))
            //       {
            //         z=z_max;
            //       } 
            //  }
            //  else{
            //     if(((int&)z)  > ((int&)z_max ))
            //      {
            //         z = z_max;
            //      }
            //  }


           //  std::cout<<(int)z<<endl;
        }
    }
    else{ //gr
        if(((sign_x == 0x1) && (exp_x == 0x00) && (val_x == 0x00)) || ((sign_y == 0x1) && (exp_y == 0x00) && (val_y == 0x00)))//nan
        {
            self_float ret;
            ret.value = 0x7fc00000;
            z= ret.fp32;
            //z = (float)0x7fc00000;
        }
        else{
             _fp8_143_to_float(x, gr, 7, (uint32_t*)(&z_32_x));//gr 43 
             _fp8_143_to_float(y, gr, 7,(uint32_t*)(&z_32_y));
             //z_mul = z_32_x * z_32_y;
             //z_mul=subnormal(subnormal(z_32_x) * subnormal(z_32_y));
             // std::cout<<z_32_x<<endl;
             //std::cout<<z_mul<<endl;
             self_float z_32_x2;
             self_float z_32_y2;
             z_32_x2.value = z_32_x;
             z_32_y2.value = z_32_y;
              // printf("z_32_x2 =  %f\n", z_32_x2.fp32);
             uint8_t x_max= sign_x<<7| 0x7f; //max_normal
             uint8_t y_max = sign_y<<7 | 0x7f;
             
             _fp8_143_to_float(&x_max, gr, 7, (uint32_t*)(&z_32_x_max));//nv 43 
             _fp8_143_to_float(&y_max, gr, 7,(uint32_t*)(&z_32_y_max));
             //self_float z_32_x_max2;
             //self_float z_32_y_max2;
             //z_32_x_max2.value = z_32_x_max;
             //z_32_y_max2.value = z_32_y_max;
             //z_max = z_32_x_max2.fp32 *  z_32_y_max2.fp32;
            
             z_mul=z_32_x2.fp32  * z_32_y2.fp32;//z_mul=subnormal(subnormal(z_32_x) * subnormal(z_32_y)); //
            //  printf("z_mul =  %f\n", z_mul);

             z=subnormal(f32_to_bf24(z_mul));
             //lhs.fp32 = f32_to_bf24(z_mul);
            
            //  if((sign_x ^ sign_y)==1)//overflow
            //  {
            //     if(((int&)z - 0x80000000) > ((int&)z_max))
            //       {
            //         z=z_max;
            //       } 
            //  }
            //  else{
            //     if(((int&)z)  > ((int&)z_max ))
            //      {
            //         z = z_max;
            //      }
            //  }
       // std::cout<<(int)z<<endl;
        }
    }

}

//----152---152------
void mul_fp8_152_fp8_152_to_fp24(const uint8_t *x, const uint8_t *y, float& z, fp8_format nv_or_gr)
{
    uint8_t sign_x = 0x1 & (x[0]>>7);
    uint8_t exp_x  = 0x1F & (x[0] >> 2);
    uint8_t val_x  = 0x03 & x[0];

    uint8_t sign_y = 0x1 & (y[0]>>7);
    uint8_t exp_y = 0x1F & (y[0] >> 2);
    uint8_t val_y = 0x03 & y[0];

    // float z_32_x;
    // float z_32_y;
     float z_mul;
    // float z_32_x_max;
    // float z_32_y_max;
    // float z_max;
    uint32_t z_32_x;
    uint32_t z_32_y;
    // uint32_t z_mul;
    uint32_t z_32_x_max;
    uint32_t z_32_y_max;
    //uint32_t z_max;
	
    if(nv_or_gr==nv) //nv 52
    {
        if(((exp_x == 0x1f) && (val_x != 0x00)) || ((exp_y == 0x1f) && (val_y != 0x00)))//nan
        {
             self_float ret;
            ret.value = 0x7fc00000;
            z= ret.fp32;
            //z.value = 0x7fc000; //
           // z= 0x7fc00000;
            
        }
        else if((exp_x == 0x1f) || (exp_y == 0x1f))
        {        
            if(((exp_x == 0x1f) && (val_x == 0x00) && (exp_y == 0x00 /*&& val_y ==0x00*/)) || ((exp_y == 0x1f) && (val_y == 0x00) && (exp_x == 0x00 /*&& val_y ==0x00*/)))//inf and 0/subnormal
            {
                self_float ret;
                ret.value = 0x7fc00000;
                z= ret.fp32;
                //z = 0x7fc00000;//nan
            }
        
            else if(((exp_x == 0x1f) && ((int)val_x == 0)&& ((int)exp_y !=31) && ((int)exp_y !=0)) || ((exp_y == 0x1f) && (val_y == 0x00)&& ((int)exp_x !=31) && ((int)exp_x !=0)))//inf *normal
            {
                 if((int)sign_x ==(int)sign_y)
                    {
                        self_float ret;
                        ret.value = 0x7f800000;
                        z= ret.fp32;
                    }
                    else{
                        self_float ret;
                        ret.value = 0xff800000;
                        z = ret.fp32;
                    }
            }
            else if((exp_x == 0x1f) && (exp_y == 0x1f))
            {
                  if((int)sign_x ==(int)sign_y)
                    {
                        self_float ret;
                        ret.value = 0x7f800000;
                        z= ret.fp32;
                    }
                    else{
                        self_float ret;
                        ret.value = 0xff800000;
                        z = ret.fp32;
                    }

            }
        }
        else{
             _fp8_152_to_float(x, nv, 15, (uint32_t*)(&z_32_x));//nv 52
             _fp8_152_to_float(y, nv, 15, (uint32_t*)(&z_32_y));
             //z_mul = z_32_x * z_32_y; 
             self_float z_32_x1;
             self_float z_32_y1;
             z_32_x1.value = z_32_x;
             z_32_y1.value = z_32_y;
             z_mul=subnormal(subnormal(z_32_x1.fp32) * subnormal(z_32_y1.fp32));
             //std::cout<<z_32_x<<endl;
             //std::cout<<z_32_y<<endl;
             //std::cout<<z_mul<<endl;
            
             uint8_t x_max= sign_x<<7| 0x7b; //max_normal
             uint8_t y_max = sign_y<<7 | 0x7b;
             
             _fp8_152_to_float(&x_max, nv, 15, (uint32_t*)(&z_32_x_max));//nv 52
             _fp8_152_to_float(&y_max, nv, 15, (uint32_t*)(&z_32_y_max));
             //self_float z_32_x_max1;
             //self_float z_32_y_max1;
             //z_32_x_max1.value = z_32_x_max;
             //z_32_y_max1.value = z_32_y_max;

             //z_max = z_32_x_max1.fp32 *  z_32_y_max1.fp32;
             /*if((sign_x ^ sign_y) ==1)//overflow
             {
                if(((int&)z_mul - 0x80000000) > ((int&)z_max))
                  {
                    z_mul=z_max;
                  } 
             }
             else{
                if(((int&)z_mul)  > ((int&)z_max ))
                 {
                    z_mul = z_max;
                 }
             }*/
              //_float_to_bf24(z_mul, (uint8_t*)(&z.value));
              //z.value=f32_to_bf24(z_mul);
              z = subnormal(f32_to_bf24(z_mul));
             //lhs.fp32 = f32_to_bf24(z_mul);
            
            //  if((sign_x ^ sign_y) ==1)//overflow
            //  {
            //     if(((int&)z - 0x80000000) > ((int&)z_max))
            //       {
            //         z=z_max;
            //       } 
            //  }
            //  else{
            //     if(((int&)z)  > ((int&)z_max ))
            //      {
            //         z = z_max;
            //      }
            //  }
            // std::cout<<z<<endl;
        }
    }
    else{ //gr
        if(((sign_x == 0x1) && (exp_x == 0x00) && (val_x == 0x00)) || ((sign_y == 0x1) && (exp_y == 0x00) && (val_y == 0x00)))//nan
        {
            self_float ret;
            ret.value = 0x7fc00000;
            z= ret.fp32;
            //z = (float)0x7fc00000;
     
        }
        else{
             _fp8_152_to_float(x, gr, 15, (uint32_t*)(&z_32_x));//gr 52 
             _fp8_152_to_float(y, gr, 15,(uint32_t*)(&z_32_y));
            
             self_float z_32_x2;
             self_float z_32_y2;
             z_32_x2.value = z_32_x;
             z_32_y2.value = z_32_y;
             z_mul=z_32_x2.fp32 * z_32_y2.fp32;//z_mul=subnormal(subnormal(z_32_x) * subnormal(z_32_y)); //
             //std::cout<<z_32_x<<endl;
             //std::cout<<z_32_y<<endl;
            //std::cout<<z_mul<<endl;

             uint8_t x_max= sign_x<<7| 0x7f; //max_normal
             uint8_t y_max = sign_y<<7 | 0x7f;
             
             _fp8_152_to_float(&x_max, gr, 15, (uint32_t*)(&z_32_x_max));//gr 52
             _fp8_152_to_float(&y_max, gr, 15,(uint32_t*)(&z_32_y_max));
             //self_float z_32_x_max2;
             //self_float z_32_y_max2;
             //z_32_x_max2.value = z_32_x_max;
             //z_32_y_max2.value = z_32_y_max;
             //z_max = z_32_x_max2.fp32 *  z_32_y_max2.fp32;
             /*if((sign_x ^sign_y)==1)//overflow
             {
                if(((int&)z_mul - 0x80000000) > ((int&)z_max)) //abs()
                  {
                    z_mul=z_max;
                  } 
             }
             else{
                if(((int&)z_mul)  > ((int&)z_max ))
                 {
                    z_mul = z_max;
                 }
             }*/
             //_float_to_bf24(z_mul,(uint8_t*)(&z.value));
             //std::cout<<z_mul<<endl;
             //float a;
              //z.value=f32_to_bf24(z_mul); 
              z = subnormal(f32_to_bf24(z_mul)); 
               //z = z_mul;
             // std::cout<<z<<endl;
             // z.value = a;
             // std::cout<<z.value<<endl;
             //lhs.fp32 = f32_to_bf24(z_mul);
            
            //   if((sign_x ^sign_y)==1)//overflow
            //  {
            //     if(((int&)z - 0x80000000) > ((int&)z_max)) //abs()
            //       {
            //         z =z_max;
            //       } 
            //  }
            //  else{
            //     if(((int&)z)  > ((int&)z_max ))
            //      {
            //         z = z_max;
            //      }
            //  }
      
        }
    
    }

}

//---------mul----152---143-------------------------------
void mul_fp8_152_fp8_143_to_fp24(const uint8_t *x, const uint8_t *y, float& z, fp8_format nv_or_gr) //152*143
{
    //152
    uint8_t sign_x = 0x1 & (x[0]>>7);
    uint8_t exp_x  = 0x1F & (x[0] >> 2);
    uint8_t val_x  = 0x03 & x[0];
    //143
    uint8_t sign_y = 0x1 & (y[0]>>7);
    uint8_t exp_y = 0x0F & (y[0] >> 3);
    uint8_t val_y = 0x07 & y[0];
    //--
    // float z_32_x;
    // float z_32_y;
     float z_mul;
    // float z_32_x_max;
    // float z_32_y_max;
    // float z_max;
    uint32_t z_32_x;
    uint32_t z_32_y;
    // uint32_t z_mul;
    uint32_t z_32_x_max;
    uint32_t z_32_y_max;
    //uint32_t z_max;
    
	
    if(nv_or_gr==nv) //nv 52 43
    {
        
        // if(exp_x== 0x1f || ((exp_y == 0xf) && (val_y == 0x07)) )
       // {
            if(((exp_x == 0x1F) && ((int)val_x != 0)) || ((exp_y == 0xF) && (val_y == 0x07)))//nan
            {
                self_float ret;
                ret.value = 0x7fc00000;
                z= ret.fp32;
                //z = 0x7fc00000; //
                
            }

            else if((exp_x == 0x1F) && ((int)val_x == 0) && (exp_y == 0x00) )//|| (((exp_y == 0x1f) && (val_y = 0x0)) && ((exp_y == 0x0 && val_y ==0x0))))//inf and 0/subnormal
            {
                self_float ret;
                ret.value = 0x7fc00000;
                z= ret.fp32;
                    //z = 0x7fc00000;//nan
            }
            else if((exp_x == 0x1f) && ((int)val_x == 0) && ((int)exp_y != 0))//&&((int)val_y != 7))//inf* normal 
            {         
                        
                        if(((int)exp_y == 15) && ((int)exp_x== 7))
                        {
                           self_float ret0;
                            ret0.value = 0x7fc00000;
                            z= ret0.fp32;

                        }
                        else if((int)sign_x == (int)sign_y)
                        {
                            self_float ret0;
                            ret0.value = 0x7f800000;
                            z= ret0.fp32;
                        }
                        else{
                        self_float ret1;
                        ret1.value = 0xff800000;
                            z= ret1.fp32;
                        }
                     
            }
            
           /* else if ((exp_x == 0x1f) && (val_x = 0x00) && (exp_y != 0x00) && (val_y != 0x00)) //inf*normal
            {
            
                    if(sign_x == sign_y)
                    {
                        self_float ret;
                        ret.value = 0x7f800000;
                        z= ret.fp32;
                    }
                    else{
                    self_float ret1;
                    ret1.value = 0xff800000;
                        z= ret1.fp32;
                    }
            }*/

        
        else{
             _fp8_152_to_float(x, nv, 15, (uint32_t*)(&z_32_x));//nv 52
             _fp8_143_to_float(y, nv, 7, (uint32_t*)(&z_32_y));//43
             //z_mul = z_32_x * z_32_y;
             self_float z_32_x1;
             self_float z_32_y1;
             z_32_x1.value = z_32_x;
             z_32_y1.value = z_32_y;

             z_mul=subnormal(subnormal( z_32_x1.fp32) * subnormal(z_32_y1.fp32));
            // std::cout<<z_mul<<endl;
             uint8_t x_max= sign_x<<7| 0x7b; //max_normal
             uint8_t y_max = sign_y<<7 | 0x7e;
             
             _fp8_152_to_float(&x_max, nv, 15, (uint32_t*)(&z_32_x_max));//nv 52
             _fp8_143_to_float(&y_max, nv, 7, (uint32_t*)(&z_32_y_max));
             //self_float z_32_x_max1;
             //self_float z_32_y_max1;
             //z_32_x_max1.value = z_32_x_max;
             //z_32_y_max1.value = z_32_y_max;
             //z_max = z_32_x_max1.fp32 *  z_32_y_max1.fp32;
             /*if((sign_x ^ sign_y)==1)//overflow
             {
                if(((int&)z_mul - 0x80000000) > ((int&)z_max))
                  {
                    z_mul=z_max;
                  } 
             }
             else{
                if(((int&)z_mul)  > ((int&)z_max ))
                 {
                    z_mul = z_max;
                 }
             }*/
             // _float_to_bf24(z_mul, (uint8_t*)(&z.value));
             z = subnormal(f32_to_bf24(z_mul));
             //lhs.fp32 = f32_to_bf24(z_mul);
           
            //  if((sign_x ^ sign_y)==1)//overflow
            //  {
            //     if(((int&)z - 0x80000000) > ((int&)z_max))
            //       {
            //         z=z_max;
            //       } 
            //  }
            //  else{
            //     if(((int&)z)  > ((int&)z_max ))
            //      {
            //         z = z_max;
            //      }
            //  }
             //std::cout<<z<<endl;
        }
    }
    else{ //gr
        if (((sign_x == 0x1) && (exp_x == 0x00) && (val_x == 0x00)) || ((sign_y == 0x1) && (exp_y == 0x00) && (val_y == 0x00)))//nan
        {
           self_float ret;
            ret.value = 0x7fc00000;
            z= ret.fp32;
           // z = 0x7fc00000;
        }
        else{
             _fp8_152_to_float(x, gr, 15, (uint32_t*)(&z_32_x));//gr 52 
             _fp8_143_to_float(y, gr, 7, (uint32_t*)(&z_32_y));//43
             
             self_float z_32_x2;
             self_float z_32_y2;
             z_32_x2.value = z_32_x;
             z_32_y2.value = z_32_y;

             z_mul=z_32_x2.fp32 * z_32_y2.fp32;//z_mul=subnormal(subnormal(z_32_x) * subnormal(z_32_y)); //
             //z_mul=subnormal(subnormal(z_32_x) * subnormal(z_32_y));
             uint8_t x_max= sign_x<<7| 0x7f; //max_normal
             uint8_t y_max = sign_y<<7 | 0x7f;
             //std::cout<<z_mul<<endl;
             _fp8_152_to_float(&x_max, gr, 15, (uint32_t*)(&z_32_x_max));//gr 52
             _fp8_143_to_float(&y_max, gr, 7,(uint32_t*)(&z_32_y_max));//43
             
             //self_float z_32_x_max2;
             //self_float z_32_y_max2;
             //z_32_x_max2.value = z_32_x_max;
             //z_32_y_max2.value = z_32_y_max;

             //z_max = z_32_x_max2.fp32 *  z_32_y_max2.fp32;
             /*if((sign_x ^ sign_y)==1)//overflow
             {
                if(((int&)z_mul - 0x80000000) > ((int&)z_max))
                  {
                    z_mul=z_max;
                  } 
             }
             else{
                if(((int&)z_mul)  > ((int&)z_max ))
                 {
                    z_mul = z_max;
                 }
             }*/
             //_float_to_bf24(z_mul,(uint8_t*)(&z.value));
              z = subnormal(f32_to_bf24(z_mul));
             //lhs.fp32 = f32_to_bf24(z_mul);
            //   if((sign_x ^ sign_y)==1)//overflow
            //  {
            //     if(((int&)z - 0x80000000) > ((int&)z_max))
            //       {
            //         z = z_max;
            //       } 
            //  }
            //  else{
            //     if(((int&)z)  > ((int&)z_max ))
            //      {
            //         z = z_max;
            //      }
            //  }
             
         //std::cout<<z<<endl;
        }
    }

}