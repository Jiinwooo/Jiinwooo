// Photoshop Touch — libTTPixelExtensionAndroid.so: JNI entry points (Ghidra decompiled)
// Port contract for the arm64 reimplementation. 122 functions.

// ==== Java_com_adobe_ttpixel_extension_gl_ECGLFilter_programUse @ 00043298 ====


undefined4
Java_com_adobe_ttpixel_extension_gl_ECGLFilter_programUse
          (int *param_1,undefined4 param_2,int param_3,int param_4,undefined4 param_5)

{
  undefined4 uVar1;
  undefined4 uVar2;
  int *piVar3;
  
  if (param_3 == 0 || param_4 == 0) {
    return 0xfffffc13;
  }
  if ((**(int **)(DAT_00043328 + 0x432c4) != 0) &&
     (piVar3 = *(int **)(DAT_0004332c + 0x432e4), *piVar3 != 0)) {
    uVar1 = (**(code **)(*param_1 + 0x194))(param_1,param_4);
    uVar2 = (**(code **)(*param_1 + 0x194))(param_1,param_3,*piVar3);
    uVar1 = ECGLProgramUse(uVar2,uVar1,param_5);
    return uVar1;
  }
  return 0xfffff830;
}

// ==== Java_com_adobe_ttpixel_extension_gl_ECGLFilter_programDispose @ 00043330 ====


undefined4
Java_com_adobe_ttpixel_extension_gl_ECGLFilter_programDispose
          (int *param_1,undefined4 param_2,int param_3,int param_4,undefined4 param_5)

{
  undefined4 uVar1;
  undefined4 uVar2;
  int *piVar3;
  
  if (param_3 == 0 || param_4 == 0) {
    return 0xfffffc13;
  }
  if ((**(int **)(DAT_000433c0 + 0x4335c) != 0) &&
     (piVar3 = *(int **)(DAT_000433c4 + 0x4337c), *piVar3 != 0)) {
    uVar1 = (**(code **)(*param_1 + 0x194))(param_1,param_4);
    uVar2 = (**(code **)(*param_1 + 0x194))(param_1,param_3,*piVar3);
    uVar1 = ECGLProgramDispose(uVar2,uVar1,param_5);
    return uVar1;
  }
  return 0xfffff830;
}

// ==== Java_com_adobe_ttpixel_extension_gl_ECGLFilter_programCreate @ 000433c8 ====


undefined4
Java_com_adobe_ttpixel_extension_gl_ECGLFilter_programCreate
          (int *param_1,undefined4 param_2,int param_3,int param_4,undefined4 param_5,
          undefined4 param_6)

{
  undefined4 uVar1;
  undefined4 uVar2;
  int *piVar3;
  
  if (param_3 == 0 || param_4 == 0) {
    uVar1 = 0xfffffc13;
  }
  else {
    if ((**(int **)(DAT_00043484 + 0x43410) != 0) &&
       (piVar3 = *(int **)(DAT_00043488 + 0x43430), *piVar3 != 0)) {
      uVar1 = (**(code **)(*param_1 + 0x194))(param_1,param_4);
      uVar2 = (**(code **)(*param_1 + 0x194))(param_1,param_3,*piVar3);
      uVar1 = ECGLProgramCreate(uVar2,uVar1,param_5,param_6);
      return uVar1;
    }
    uVar1 = 0xfffff830;
  }
  return uVar1;
}

// ==== Java_com_adobe_ttpixel_extension_gl_ECGLFilter_shaderDispose @ 0004348c ====


undefined4
Java_com_adobe_ttpixel_extension_gl_ECGLFilter_shaderDispose
          (int *param_1,undefined4 param_2,int param_3,int param_4,undefined4 param_5)

{
  undefined4 uVar1;
  undefined4 uVar2;
  int *piVar3;
  
  if (param_3 == 0 || param_4 == 0) {
    return 0xfffffc13;
  }
  if ((**(int **)(DAT_0004351c + 0x434b8) != 0) &&
     (piVar3 = *(int **)(DAT_00043520 + 0x434d8), *piVar3 != 0)) {
    uVar1 = (**(code **)(*param_1 + 0x194))(param_1,param_4);
    uVar2 = (**(code **)(*param_1 + 0x194))(param_1,param_3,*piVar3);
    uVar1 = ECGLShaderDispose(uVar2,uVar1,param_5);
    return uVar1;
  }
  return 0xfffff830;
}

// ==== Java_com_adobe_ttpixel_extension_gl_ECGLFilter_shaderCreate @ 00043524 ====


undefined4
Java_com_adobe_ttpixel_extension_gl_ECGLFilter_shaderCreate
          (int *param_1,undefined4 param_2,int param_3,int param_4,undefined4 param_5,
          undefined4 param_6,undefined4 param_7)

{
  undefined4 uVar1;
  int iVar2;
  undefined4 uVar3;
  int *piVar4;
  
  if (param_3 == 0 || param_4 == 0) {
    uVar3 = 0xfffffc13;
  }
  else if ((**(int **)(DAT_00043620 + 0x43568) == 0) ||
          (piVar4 = *(int **)(DAT_00043624 + 0x43588), *piVar4 == 0)) {
    uVar3 = 0xfffff830;
  }
  else {
    uVar3 = (**(code **)(*param_1 + 0x194))(param_1,param_4);
    uVar1 = (**(code **)(*param_1 + 0x194))(param_1,param_3,*piVar4);
    iVar2 = (**(code **)(*param_1 + 0x2a4))(param_1,param_7,param_3 == 0 || param_4 == 0);
    if (iVar2 == 0) {
      uVar3 = 0xfffffc16;
    }
    else {
      uVar3 = ECGLShaderCreateEx(uVar1,uVar3,param_5,param_6,iVar2);
      (**(code **)(*param_1 + 0x2a8))(param_1,param_7,iVar2);
    }
  }
  return uVar3;
}

// ==== Java_com_adobe_ttpixel_extension_gl_ECGLContext_getStringInfo @ 00043628 ====


/* WARNING: Removing unreachable block (ram,0x00043690) */

undefined4
Java_com_adobe_ttpixel_extension_gl_ECGLContext_getStringInfo
          (int *param_1,undefined4 param_2,int param_3,undefined4 param_4)

{
  undefined4 uVar1;
  
  if ((param_3 != 0) && (**(int **)(DAT_000436a4 + 0x43654) != 0)) {
    uVar1 = (**(code **)(*param_1 + 0x194))();
    ECGLGetStringInfo(uVar1,param_4);
  }
  return 0;
}

// ==== Java_com_adobe_ttpixel_extension_gl_ECGLFilter_textureUploadData @ 000436a8 ====


undefined4
Java_com_adobe_ttpixel_extension_gl_ECGLFilter_textureUploadData
          (int *param_1,undefined4 param_2,int param_3,int param_4,undefined4 param_5,
          undefined4 param_6,undefined4 param_7,int param_8,int param_9,undefined4 param_10,
          int param_11)

{
  int iVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  int *piVar4;
  int iVar5;
  uint uVar6;
  undefined8 uVar7;
  
  if ((param_3 != 0 && param_4 != 0) && (param_11 != 0)) {
    if ((**(int **)(DAT_00043830 + 0x43718) == 0) ||
       (piVar4 = *(int **)(DAT_00043834 + 0x4373c), *piVar4 == 0)) {
      return 0xfffff830;
    }
    uVar2 = (**(code **)(*param_1 + 0x194))(param_1,param_4,**(int **)(DAT_00043830 + 0x43718));
    uVar3 = (**(code **)(*param_1 + 0x194))(param_1,param_3,*piVar4);
    uVar7 = (**(code **)(*param_1 + 0x39c))(param_1,param_11);
    iVar5 = (int)((ulonglong)uVar7 >> 0x20);
    uVar6 = param_9 * param_8 * 4;
    iVar1 = (int)uVar6 >> 0x1f;
    if ((iVar1 == iVar5 || iVar1 < iVar5) &&
       ((iVar1 != iVar5 || (uVar6 < (uint)uVar7 || uVar6 - (uint)uVar7 == 0)))) {
      (**(code **)(*param_1 + 0x398))(param_1,param_11);
      uVar2 = ECGLTextureUploadData(uVar3,uVar2,param_5,param_6);
      return uVar2;
    }
  }
  return 0xfffffc13;
}

// ==== Java_com_adobe_ttpixel_extension_gl_ECGLFilter_texturePrepare @ 00043838 ====


undefined4
Java_com_adobe_ttpixel_extension_gl_ECGLFilter_texturePrepare
          (int *param_1,undefined4 param_2,int param_3,int param_4,undefined4 param_5,
          undefined4 param_6)

{
  undefined4 uVar1;
  undefined4 uVar2;
  int *piVar3;
  
  if (param_3 == 0 || param_4 == 0) {
    uVar1 = 0xfffffc13;
  }
  else {
    if ((**(int **)(DAT_00043918 + 0x43884) != 0) &&
       (piVar3 = *(int **)(DAT_0004391c + 0x438a4), *piVar3 != 0)) {
      uVar1 = (**(code **)(*param_1 + 0x194))(param_1,param_4,**(int **)(DAT_00043918 + 0x43884));
      uVar2 = (**(code **)(*param_1 + 0x194))(param_1,param_3,*piVar3);
      uVar1 = ECGLTexturePrepare(uVar2,uVar1,param_5,param_6);
      return uVar1;
    }
    uVar1 = 0xfffff830;
  }
  return uVar1;
}

// ==== Java_com_adobe_ttpixel_extension_gl_ECGLFilter_asyncReadPixels @ 00043920 ====


int Java_com_adobe_ttpixel_extension_gl_ECGLFilter_asyncReadPixels
              (int *param_1,undefined4 param_2,int param_3,int param_4,int param_5)

{
  int iVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  int *piVar4;
  undefined8 uVar5;
  int local_20;
  int iStack_1c;
  
  if ((param_3 == 0 || param_4 == 0) || (param_5 == 0)) {
LAB_00043940:
    iVar1 = -0x3ed;
  }
  else {
    piVar4 = *(int **)(DAT_00043a28 + 0x43960);
    if ((*piVar4 != 0) && (**(int **)(DAT_00043a2c + 0x43980) != 0)) {
      uVar2 = (**(code **)(*param_1 + 0x194))(param_1,param_4);
      uVar3 = (**(code **)(*param_1 + 0x194))(param_1,param_3,*piVar4);
      iVar1 = ECGLGetImageDimensions(uVar3,uVar2,&iStack_1c,&local_20);
      if (iVar1 != 0) {
        return iVar1;
      }
      uVar5 = (**(code **)(*param_1 + 0x39c))(param_1,param_5);
      if (((int)uVar5 != local_20 * iStack_1c * 4) || ((int)((ulonglong)uVar5 >> 0x20) != 0))
      goto LAB_00043940;
      iVar1 = (**(code **)(*param_1 + 0x398))(param_1,param_5);
      if (iVar1 != 0) {
        iVar1 = ECGLAsyncReadPixels(uVar3,uVar2);
        return iVar1;
      }
    }
    iVar1 = -2000;
  }
  return iVar1;
}

// ==== Java_com_adobe_ttpixel_extension_gl_ECGLFilter_asyncJoin @ 00043a30 ====


undefined4
Java_com_adobe_ttpixel_extension_gl_ECGLFilter_asyncJoin
          (int *param_1,undefined4 param_2,int param_3,int param_4)

{
  undefined4 uVar1;
  undefined4 uVar2;
  int *piVar3;
  
  if (param_3 == 0 || param_4 == 0) {
    return 0xfffffc13;
  }
  piVar3 = *(int **)(DAT_00043ab8 + 0x43a58);
  if ((*piVar3 != 0) && (**(int **)(DAT_00043abc + 0x43a78) != 0)) {
    uVar1 = (**(code **)(*param_1 + 0x194))(param_1,param_4);
    uVar2 = (**(code **)(*param_1 + 0x194))(param_1,param_3,*piVar3);
    uVar1 = ECGLAsyncJoin(uVar2,uVar1);
    return uVar1;
  }
  return 0xfffff830;
}

// ==== Java_com_adobe_ttpixel_extension_gl_ECGLFilter_asyncInterrupt @ 00043ac0 ====


undefined4
Java_com_adobe_ttpixel_extension_gl_ECGLFilter_asyncInterrupt
          (int *param_1,undefined4 param_2,int param_3,int param_4)

{
  undefined4 uVar1;
  undefined4 uVar2;
  int *piVar3;
  
  if (param_3 == 0 || param_4 == 0) {
    return 0xfffffc13;
  }
  piVar3 = *(int **)(DAT_00043b48 + 0x43ae8);
  if ((*piVar3 != 0) && (**(int **)(DAT_00043b4c + 0x43b08) != 0)) {
    uVar1 = (**(code **)(*param_1 + 0x194))(param_1,param_4);
    uVar2 = (**(code **)(*param_1 + 0x194))(param_1,param_3,*piVar3);
    uVar1 = ECGLAsyncInterrupt(uVar2,uVar1);
    return uVar1;
  }
  return 0xfffff830;
}

// ==== Java_com_adobe_ttpixel_extension_gl_ECGLFilter_asyncExecuteSequence @ 00043b50 ====


int Java_com_adobe_ttpixel_extension_gl_ECGLFilter_asyncExecuteSequence
              (int *param_1,undefined4 param_2,int param_3,int param_4,int param_5,int param_6)

{
  undefined4 uVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  int iVar6;
  int *piVar7;
  
  if ((param_3 == 0 || param_4 == 0) || (param_5 == 0 || param_6 == 0)) {
    iVar6 = -0x3ed;
  }
  else {
    piVar7 = *(int **)(DAT_00043c80 + 0x43b9c);
    if ((*piVar7 == 0) || (**(int **)(DAT_00043c84 + 0x43bbc) == 0)) {
      iVar6 = -2000;
    }
    else {
      uVar1 = (**(code **)(*param_1 + 0x194))(param_1,param_4);
      uVar2 = (**(code **)(*param_1 + 0x194))(param_1,param_3,*piVar7);
      uVar3 = (**(code **)(*param_1 + 0x39c))(param_1,param_5);
      uVar4 = (**(code **)(*param_1 + 0x398))(param_1,param_5);
      uVar5 = (**(code **)(*param_1 + 0x54))(param_1,param_6);
      iVar6 = ECGLAsyncExecuteSequence(uVar2,uVar1,uVar4,uVar3,DAT_00043c88 + 0x43c50,uVar5);
      if (iVar6 != 0) {
        (**(code **)(*param_1 + 0x58))(param_1,uVar5);
      }
    }
  }
  return iVar6;
}

// ==== Java_com_adobe_ttpixel_extension_gl_ECGLFilter_executeSequence @ 00043c8c ====


undefined4
Java_com_adobe_ttpixel_extension_gl_ECGLFilter_executeSequence
          (int *param_1,undefined4 param_2,int param_3,int param_4,int param_5)

{
  undefined4 uVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  int *piVar5;
  
  if ((param_3 == 0 || param_4 == 0) || (param_5 == 0)) {
    return 0xfffffc13;
  }
  piVar5 = *(int **)(DAT_00043d60 + 0x43cc4);
  if ((*piVar5 != 0) && (**(int **)(DAT_00043d64 + 0x43ce4) != 0)) {
    uVar1 = (**(code **)(*param_1 + 0x194))(param_1,param_4);
    uVar2 = (**(code **)(*param_1 + 0x194))(param_1,param_3,*piVar5);
    uVar3 = (**(code **)(*param_1 + 0x39c))(param_1,param_5);
    uVar4 = (**(code **)(*param_1 + 0x398))(param_1,param_5);
    uVar1 = ECGLExecuteSequence(uVar2,uVar1,uVar4,uVar3);
    return uVar1;
  }
  return 0xfffff830;
}

// ==== Java_com_adobe_ttpixel_extension_gl_ECGLFilter_setParamFloatMatrix @ 00043d68 ====


undefined4
Java_com_adobe_ttpixel_extension_gl_ECGLFilter_setParamFloatMatrix
          (int *param_1,undefined4 param_2,int param_3,int param_4,int param_5,int param_6,
          int param_7,int param_8)

{
  undefined4 uVar1;
  int iVar2;
  int iVar3;
  undefined4 uVar4;
  int *piVar5;
  
  if ((param_3 == 0 || param_4 == 0) || (param_5 == 0 || param_6 == 0)) {
    uVar4 = 0xfffffc13;
  }
  else {
    piVar5 = *(int **)(DAT_00043ef0 + 0x43dc0);
    if ((*piVar5 == 0) || (**(int **)(DAT_00043ef4 + 0x43de0) == 0)) {
      uVar4 = 0xfffff830;
    }
    else {
      uVar4 = (**(code **)(*param_1 + 0x194))(param_1,param_4);
      uVar1 = (**(code **)(*param_1 + 0x194))(param_1,param_3,*piVar5);
      iVar2 = (**(code **)(*param_1 + 0x2a4))(param_1,param_5,0);
      if (iVar2 == 0) {
        uVar4 = 0xfffffc16;
      }
      else {
        iVar3 = (**(code **)(*param_1 + 0x2ac))(param_1,param_6);
        if (iVar3 == param_8 * param_7 * param_7) {
          iVar3 = (**(code **)(*param_1 + 0x2f4))(param_1,param_6,0);
          if (iVar3 == 0) {
            uVar4 = 0xfffff830;
          }
          else {
            uVar4 = ECGLSetParamFloatMatrix(uVar1,uVar4,iVar2,iVar3,param_7,param_8);
            (**(code **)(*param_1 + 0x314))(param_1,param_6,iVar3,2);
          }
        }
        else {
          uVar4 = 0xfffffc13;
        }
        (**(code **)(*param_1 + 0x2a8))(param_1,param_5,iVar2);
      }
    }
  }
  return uVar4;
}

// ==== Java_com_adobe_ttpixel_extension_gl_ECGLFilter_clear @ 00043ef8 ====


undefined4
Java_com_adobe_ttpixel_extension_gl_ECGLFilter_clear
          (int *param_1,undefined4 param_2,int param_3,int param_4,undefined4 param_5)

{
  undefined4 uVar1;
  undefined4 uVar2;
  int *piVar3;
  
  if (param_3 == 0 || param_4 == 0) {
    return 0xfffffc13;
  }
  piVar3 = *(int **)(DAT_00043f88 + 0x43f24);
  if ((*piVar3 != 0) && (**(int **)(DAT_00043f8c + 0x43f44) != 0)) {
    uVar1 = (**(code **)(*param_1 + 0x194))(param_1,param_4);
    uVar2 = (**(code **)(*param_1 + 0x194))(param_1,param_3,*piVar3);
    uVar1 = ECGLClear(uVar2,uVar1,param_5);
    return uVar1;
  }
  return 0xfffff830;
}

// ==== Java_com_adobe_ttpixel_extension_gl_ECGLFilter_readPixels @ 00043f90 ====


int Java_com_adobe_ttpixel_extension_gl_ECGLFilter_readPixels
              (int *param_1,undefined4 param_2,int param_3,int param_4,int param_5)

{
  int iVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  int *piVar4;
  undefined8 uVar5;
  int local_20;
  int iStack_1c;
  
  if ((param_3 == 0 || param_4 == 0) || (param_5 == 0)) {
LAB_00043fb0:
    iVar1 = -0x3ed;
  }
  else {
    piVar4 = *(int **)(DAT_00044098 + 0x43fd0);
    if ((*piVar4 != 0) && (**(int **)(DAT_0004409c + 0x43ff0) != 0)) {
      uVar2 = (**(code **)(*param_1 + 0x194))(param_1,param_4);
      uVar3 = (**(code **)(*param_1 + 0x194))(param_1,param_3,*piVar4);
      iVar1 = ECGLGetImageDimensions(uVar3,uVar2,&iStack_1c,&local_20);
      if (iVar1 != 0) {
        return iVar1;
      }
      uVar5 = (**(code **)(*param_1 + 0x39c))(param_1,param_5);
      if (((int)uVar5 != local_20 * iStack_1c * 4) || ((int)((ulonglong)uVar5 >> 0x20) != 0))
      goto LAB_00043fb0;
      iVar1 = (**(code **)(*param_1 + 0x398))(param_1,param_5);
      if (iVar1 != 0) {
        iVar1 = ECGLReadPixels(uVar3,uVar2);
        return iVar1;
      }
    }
    iVar1 = -2000;
  }
  return iVar1;
}

// ==== Java_com_adobe_ttpixel_extension_gl_ECGLFilter_readPixelsRect @ 000440a0 ====


undefined4
Java_com_adobe_ttpixel_extension_gl_ECGLFilter_readPixelsRect
          (int *param_1,undefined4 param_2,int param_3,int param_4,int param_5,undefined4 param_6,
          undefined4 param_7,int param_8,int param_9)

{
  undefined4 uVar1;
  undefined4 uVar2;
  int iVar3;
  int *piVar4;
  longlong lVar5;
  
  if ((param_3 == 0 || param_4 == 0) || (param_5 == 0)) {
LAB_000440d0:
    uVar1 = 0xfffffc13;
  }
  else {
    piVar4 = *(int **)(DAT_000441bc + 0x440f0);
    if ((*piVar4 != 0) && (**(int **)(DAT_000441c0 + 0x44110) != 0)) {
      uVar1 = (**(code **)(*param_1 + 0x194))(param_1,param_4);
      uVar2 = (**(code **)(*param_1 + 0x194))(param_1,param_3,*piVar4);
      lVar5 = (**(code **)(*param_1 + 0x39c))(param_1,param_5);
      if (lVar5 != param_9 * param_8 * 4) goto LAB_000440d0;
      iVar3 = (**(code **)(*param_1 + 0x398))(param_1,param_5);
      if (iVar3 != 0) {
        uVar1 = ECGLReadPixelsRect(uVar2,uVar1,param_6,param_7);
        return uVar1;
      }
    }
    uVar1 = 0xfffff830;
  }
  return uVar1;
}

// ==== Java_com_adobe_ttpixel_extension_gl_ECGLFilter_drawArrays @ 000441c4 ====


undefined4
Java_com_adobe_ttpixel_extension_gl_ECGLFilter_drawArrays
          (int *param_1,undefined4 param_2,int param_3,int param_4,undefined4 param_5,char param_6)

{
  undefined4 uVar1;
  undefined4 uVar2;
  int *piVar3;
  
  if (param_3 == 0 || param_4 == 0) {
    return 0xfffffc13;
  }
  piVar3 = *(int **)(DAT_00044260 + 0x441f4);
  if ((*piVar3 != 0) && (**(int **)(DAT_00044264 + 0x44214) != 0)) {
    uVar1 = (**(code **)(*param_1 + 0x194))(param_1,param_4);
    uVar2 = (**(code **)(*param_1 + 0x194))(param_1,param_3,*piVar3);
    if (param_6 != '\0') {
      param_6 = '\x01';
    }
    uVar1 = ECGLDrawArrays(uVar2,uVar1,param_5,param_6);
    return uVar1;
  }
  return 0xfffff830;
}

// ==== Java_com_adobe_ttpixel_extension_gl_ECGLFilter_destroyFilter @ 00044268 ====


undefined4
Java_com_adobe_ttpixel_extension_gl_ECGLFilter_destroyFilter
          (int *param_1,undefined4 param_2,int param_3,undefined4 param_4)

{
  undefined4 uVar1;
  undefined4 uVar2;
  int *piVar3;
  
  if (param_3 == 0) {
    uVar2 = 0xfffffc13;
  }
  else {
    piVar3 = *(int **)(DAT_00044320 + 0x44298);
    if ((*piVar3 == 0) || (**(int **)(DAT_00044324 + 0x442b8) == 0)) {
      uVar2 = 0xfffff830;
    }
    else {
      uVar2 = (**(code **)(*param_1 + 0x194))(param_1,param_4);
      uVar1 = (**(code **)(*param_1 + 0x194))(param_1,param_3,*piVar3);
      uVar2 = ECGLDestroyFilter(uVar1,uVar2);
      (**(code **)(*param_1 + 0x1b8))(param_1,param_3,*piVar3,*param_1,0,0);
    }
  }
  return uVar2;
}

// ==== Java_com_adobe_ttpixel_extension_gl_ECGLFilter_enableVertexAttrib @ 00044328 ====


undefined4
Java_com_adobe_ttpixel_extension_gl_ECGLFilter_enableVertexAttrib
          (int *param_1,undefined4 param_2,int param_3,int param_4,undefined4 param_5,char param_6)

{
  undefined4 uVar1;
  undefined4 uVar2;
  int *piVar3;
  
  if (param_3 == 0 || param_4 == 0) {
    return 0xfffffc13;
  }
  piVar3 = *(int **)(DAT_000443c4 + 0x44358);
  if ((*piVar3 != 0) && (**(int **)(DAT_000443c8 + 0x44378) != 0)) {
    uVar1 = (**(code **)(*param_1 + 0x194))(param_1,param_4);
    uVar2 = (**(code **)(*param_1 + 0x194))(param_1,param_3,*piVar3);
    if (param_6 != '\0') {
      param_6 = '\x01';
    }
    uVar1 = ECGLEnableVertexAttrib(uVar2,uVar1,param_5,param_6);
    return uVar1;
  }
  return 0xfffff830;
}

// ==== Java_com_adobe_ttpixel_extension_gl_ECGLFilter_setDrawMode @ 000443cc ====


undefined4
Java_com_adobe_ttpixel_extension_gl_ECGLFilter_setDrawMode
          (int *param_1,undefined4 param_2,int param_3,int param_4,undefined4 param_5)

{
  undefined4 uVar1;
  undefined4 uVar2;
  int *piVar3;
  
  if (param_3 == 0 || param_4 == 0) {
    return 0xfffffc13;
  }
  piVar3 = *(int **)(DAT_0004445c + 0x443f8);
  if ((*piVar3 != 0) && (**(int **)(DAT_00044460 + 0x44418) != 0)) {
    uVar1 = (**(code **)(*param_1 + 0x194))(param_1,param_4);
    uVar2 = (**(code **)(*param_1 + 0x194))(param_1,param_3,*piVar3);
    uVar1 = ECGLSetDrawMode(uVar2,uVar1,param_5);
    return uVar1;
  }
  return 0xfffff830;
}

// ==== Java_com_adobe_ttpixel_extension_gl_ECGLFilter_setVertexAttribDataRange @ 00044464 ====


undefined4
Java_com_adobe_ttpixel_extension_gl_ECGLFilter_setVertexAttribDataRange
          (int *param_1,undefined4 param_2,int param_3,int param_4,undefined4 param_5,
          undefined4 param_6)

{
  undefined4 uVar1;
  undefined4 uVar2;
  int *piVar3;
  
  if (param_3 == 0 || param_4 == 0) {
    return 0xfffffc13;
  }
  piVar3 = *(int **)(DAT_000444fc + 0x44494);
  if ((*piVar3 != 0) && (**(int **)(DAT_00044500 + 0x444b4) != 0)) {
    uVar1 = (**(code **)(*param_1 + 0x194))(param_1,param_4);
    uVar2 = (**(code **)(*param_1 + 0x194))(param_1,param_3,*piVar3);
    uVar1 = ECGLSetVertexAttribDataRange(uVar2,uVar1,param_5,param_6);
    return uVar1;
  }
  return 0xfffff830;
}

// ==== Java_com_adobe_ttpixel_extension_gl_ECGLFilter_getVertexAttribDataRangeCount @ 00044504 ====


undefined4
Java_com_adobe_ttpixel_extension_gl_ECGLFilter_getVertexAttribDataRangeCount
          (int *param_1,undefined4 param_2,int param_3,int param_4)

{
  undefined4 uVar1;
  undefined4 uVar2;
  int iVar3;
  int *piVar4;
  undefined4 auStack_1c [2];
  
  if (((param_4 != 0 && param_3 != 0) && (piVar4 = *(int **)(DAT_0004459c + 0x4452c), *piVar4 != 0))
     && (**(int **)(DAT_000445a0 + 0x44540) != 0)) {
    uVar1 = (**(code **)(*param_1 + 0x194))(param_1,param_4);
    uVar2 = (**(code **)(*param_1 + 0x194))(param_1,param_3,*piVar4);
    iVar3 = ECGLGetVertexAttribDataRange(uVar2,uVar1,0,auStack_1c);
    if (iVar3 == 0) {
      return auStack_1c[0];
    }
  }
  return 0;
}

// ==== Java_com_adobe_ttpixel_extension_gl_ECGLFilter_getVertexAttribDataRangeStart @ 000445a4 ====


undefined4
Java_com_adobe_ttpixel_extension_gl_ECGLFilter_getVertexAttribDataRangeStart
          (int *param_1,undefined4 param_2,int param_3,int param_4)

{
  undefined4 uVar1;
  undefined4 uVar2;
  int iVar3;
  int *piVar4;
  undefined4 auStack_1c [2];
  
  if (((param_4 != 0 && param_3 != 0) && (piVar4 = *(int **)(DAT_0004463c + 0x445cc), *piVar4 != 0))
     && (**(int **)(DAT_00044640 + 0x445e0) != 0)) {
    uVar1 = (**(code **)(*param_1 + 0x194))(param_1,param_4);
    uVar2 = (**(code **)(*param_1 + 0x194))(param_1,param_3,*piVar4);
    iVar3 = ECGLGetVertexAttribDataRange(uVar2,uVar1,auStack_1c,0);
    if (iVar3 == 0) {
      return auStack_1c[0];
    }
  }
  return 0;
}

// ==== Java_com_adobe_ttpixel_extension_gl_ECGLFilter_clearVertexAttribData @ 00044644 ====


undefined4
Java_com_adobe_ttpixel_extension_gl_ECGLFilter_clearVertexAttribData
          (int *param_1,undefined4 param_2,int param_3,int param_4,undefined4 param_5)

{
  undefined4 uVar1;
  undefined4 uVar2;
  int *piVar3;
  
  if (param_3 == 0 || param_4 == 0) {
    return 0xfffffc13;
  }
  piVar3 = *(int **)(DAT_000446d4 + 0x44670);
  if ((*piVar3 != 0) && (**(int **)(DAT_000446d8 + 0x44690) != 0)) {
    uVar1 = (**(code **)(*param_1 + 0x194))(param_1,param_4);
    uVar2 = (**(code **)(*param_1 + 0x194))(param_1,param_3,*piVar3);
    uVar1 = ECGLClearVertexAttribData(uVar2,uVar1,param_5);
    return uVar1;
  }
  return 0xfffff830;
}

// ==== Java_com_adobe_ttpixel_extension_gl_ECGLFilter_setVertexAttribDataFloat @ 000446dc ====


undefined4
Java_com_adobe_ttpixel_extension_gl_ECGLFilter_setVertexAttribDataFloat
          (int *param_1,undefined4 param_2,int param_3,int param_4,undefined4 param_5,int param_6,
          undefined4 param_7)

{
  undefined4 uVar1;
  int iVar2;
  uint uVar3;
  undefined4 uVar4;
  int *piVar5;
  undefined1 auStack_21 [5];
  
  if (param_3 != 0 && param_4 != 0) {
    piVar5 = *(int **)(DAT_000447f0 + 0x4471c);
    if ((*piVar5 == 0) || (**(int **)(DAT_000447f4 + 0x4473c) == 0)) {
      return 0xfffff830;
    }
    uVar1 = (**(code **)(*param_1 + 0x194))(param_1,param_4);
    iVar2 = (**(code **)(*param_1 + 0x194))(param_1,param_3,*piVar5);
    uVar3 = (**(code **)(*param_1 + 0x2ac))(param_1,param_7);
    if ((uint)(*(int *)(iVar2 + 0x228) * param_6) <= uVar3) {
      uVar4 = (**(code **)(*param_1 + 0x2f4))(param_1,param_7,auStack_21);
      uVar1 = ECGLSetVertexAttribDataFloat(iVar2,uVar1,param_5,param_6,uVar4);
      (**(code **)(*param_1 + 0x314))(param_1,param_7,uVar4,2);
      return uVar1;
    }
  }
  return 0xfffffc13;
}

// ==== Java_com_adobe_ttpixel_extension_gl_ECGLFilter_setNumVertices @ 000447f8 ====


undefined4
Java_com_adobe_ttpixel_extension_gl_ECGLFilter_setNumVertices
          (int *param_1,undefined4 param_2,int param_3,int param_4,undefined4 param_5)

{
  undefined4 uVar1;
  undefined4 uVar2;
  int *piVar3;
  
  if (param_3 == 0 || param_4 == 0) {
    return 0xfffffc13;
  }
  piVar3 = *(int **)(DAT_00044888 + 0x44824);
  if ((*piVar3 != 0) && (**(int **)(DAT_0004488c + 0x44844) != 0)) {
    uVar1 = (**(code **)(*param_1 + 0x194))(param_1,param_4);
    uVar2 = (**(code **)(*param_1 + 0x194))(param_1,param_3,*piVar3);
    uVar1 = ECGLSetNumVertices(uVar2,uVar1,param_5);
    return uVar1;
  }
  return 0xfffff830;
}

// ==== Java_com_adobe_ttpixel_extension_gl_ECGLFilter_getNumVertices @ 00044890 ====


undefined4
Java_com_adobe_ttpixel_extension_gl_ECGLFilter_getNumVertices
          (int *param_1,undefined4 param_2,int param_3,int param_4)

{
  undefined4 uVar1;
  undefined4 uVar2;
  int iVar3;
  int *piVar4;
  undefined4 auStack_1c [2];
  
  if (((param_4 != 0 && param_3 != 0) && (piVar4 = *(int **)(DAT_00044924 + 0x448b8), *piVar4 != 0))
     && (**(int **)(DAT_00044928 + 0x448cc) != 0)) {
    uVar1 = (**(code **)(*param_1 + 0x194))(param_1,param_4);
    uVar2 = (**(code **)(*param_1 + 0x194))(param_1,param_3,*piVar4);
    iVar3 = ECGLGetNumVertices(uVar2,uVar1,auStack_1c);
    if (iVar3 == 0) {
      return auStack_1c[0];
    }
  }
  return 0;
}

// ==== Java_com_adobe_ttpixel_extension_gl_ECGLFilter_getShaderInfoLog @ 0004492c ====


undefined4
Java_com_adobe_ttpixel_extension_gl_ECGLFilter_getShaderInfoLog
          (int *param_1,undefined4 param_2,int param_3,int param_4)

{
  undefined4 uVar1;
  undefined4 uVar2;
  int *piVar3;
  
  if (((param_3 == 0 && param_4 == 0) || (piVar3 = *(int **)(DAT_000449e8 + 0x44960), *piVar3 == 0))
     || (**(int **)(DAT_000449ec + 0x44974) == 0)) {
    uVar1 = 0;
  }
  else {
    uVar1 = (**(code **)(*param_1 + 0x194))(param_1,param_4);
    uVar2 = (**(code **)(*param_1 + 0x194))(param_1,param_3,*piVar3);
    ECGLGetShaderInfoLog(uVar2,uVar1);
    uVar1 = (**(code **)(*param_1 + 0x29c))(param_1,(int)&DAT_000449e8 + DAT_000449f0);
  }
  return uVar1;
}

// ==== Java_com_adobe_ttpixel_extension_gl_ECGLFilter_applyFilter @ 000449f4 ====


int Java_com_adobe_ttpixel_extension_gl_ECGLFilter_applyFilter
              (int *param_1,undefined4 param_2,int param_3,int param_4,int param_5,
              undefined4 param_6,char param_7)

{
  int iVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  int *piVar4;
  undefined8 uVar5;
  int local_28;
  int local_24;
  
  if ((param_3 == 0 || param_4 == 0) || (param_5 == 0)) {
LAB_00044a18:
    iVar1 = -0x3ed;
  }
  else {
    if ((**(int **)(DAT_00044b20 + 0x44a38) != 0) &&
       (piVar4 = *(int **)(DAT_00044b24 + 0x44a58), *piVar4 != 0)) {
      uVar2 = (**(code **)(*param_1 + 0x194))(param_1,param_4);
      uVar3 = (**(code **)(*param_1 + 0x194))(param_1,param_3,*piVar4);
      uVar5 = (**(code **)(*param_1 + 0x39c))(param_1,param_5);
      iVar1 = ECGLGetImageDimensions(uVar3,uVar2,&local_24,&local_28);
      if (iVar1 != 0) {
        return iVar1;
      }
      if (((int)uVar5 != local_28 * local_24 * 4) || ((int)((ulonglong)uVar5 >> 0x20) != 0))
      goto LAB_00044a18;
      iVar1 = (**(code **)(*param_1 + 0x398))(param_1,param_5);
      if (iVar1 != 0) {
        if (param_7 != '\0') {
          param_7 = '\x01';
        }
        iVar1 = ECGLApplyFilter(uVar3,uVar2,iVar1,param_6,param_7);
        return iVar1;
      }
    }
    iVar1 = -2000;
  }
  return iVar1;
}

// ==== Java_com_adobe_ttpixel_extension_gl_ECGLFilter_setParamIntMulti @ 00044b28 ====


undefined4
Java_com_adobe_ttpixel_extension_gl_ECGLFilter_setParamIntMulti
          (int *param_1,undefined4 param_2,int param_3,int param_4,int param_5,int param_6,
          int param_7)

{
  undefined4 uVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  undefined4 uVar5;
  int *piVar6;
  
  if (((param_3 == 0 || param_4 == 0) || (param_5 == 0 || param_6 == 0)) ||
     (4 < param_7 || param_7 < 1)) {
    uVar5 = 0xfffffc13;
  }
  else if ((**(int **)(DAT_00044cc0 + 0x44b94) == 0) ||
          (piVar6 = *(int **)(DAT_00044cc4 + 0x44bb4), *piVar6 == 0)) {
    uVar5 = 0xfffff830;
  }
  else {
    uVar5 = (**(code **)(*param_1 + 0x194))(param_1,param_4);
    uVar1 = (**(code **)(*param_1 + 0x194))(param_1,param_3,*piVar6);
    iVar2 = (**(code **)(*param_1 + 0x2a4))(param_1,param_5,0);
    if (iVar2 == 0) {
      uVar5 = 0xfffffc16;
    }
    else {
      iVar3 = (**(code **)(*param_1 + 0x2ac))(param_1,param_6);
      if (iVar3 == 0) {
        uVar5 = 0xfffffc13;
      }
      else {
        iVar4 = (**(code **)(*param_1 + 0x2ec))(param_1,param_6,0);
        if (iVar4 == 0) {
          uVar5 = 0xfffff830;
        }
        else {
          uVar5 = ECGLSetParamIntMulti(uVar1,uVar5,iVar2,iVar4,iVar3,param_7);
          (**(code **)(*param_1 + 0x30c))(param_1,param_6,iVar4,2);
        }
      }
      (**(code **)(*param_1 + 0x2a8))(param_1,param_5,iVar2);
    }
  }
  return uVar5;
}

// ==== Java_com_adobe_ttpixel_extension_gl_ECGLFilter_setParamInt @ 00044cc8 ====


undefined4
Java_com_adobe_ttpixel_extension_gl_ECGLFilter_setParamInt
          (int *param_1,undefined4 param_2,int param_3,int param_4,int param_5,undefined4 param_6)

{
  undefined4 uVar1;
  int iVar2;
  undefined4 uVar3;
  int *piVar4;
  
  if ((param_3 == 0 || param_4 == 0) || (param_5 == 0)) {
    uVar3 = 0xfffffc13;
  }
  else if ((**(int **)(DAT_00044dc0 + 0x44d0c) == 0) ||
          (piVar4 = *(int **)(DAT_00044dc4 + 0x44d2c), *piVar4 == 0)) {
    uVar3 = 0xfffff830;
  }
  else {
    uVar3 = (**(code **)(*param_1 + 0x194))(param_1,param_4);
    uVar1 = (**(code **)(*param_1 + 0x194))(param_1,param_3,*piVar4);
    iVar2 = (**(code **)(*param_1 + 0x2a4))(param_1,param_5,0);
    if (iVar2 == 0) {
      uVar3 = 0xfffffc16;
    }
    else {
      uVar3 = ECGLSetParamInt(uVar1,uVar3,iVar2,param_6);
      (**(code **)(*param_1 + 0x2a8))(param_1,param_5,iVar2);
    }
  }
  return uVar3;
}

// ==== Java_com_adobe_ttpixel_extension_gl_ECGLFilter_setParamFloatMulti @ 00044dc8 ====


undefined4
Java_com_adobe_ttpixel_extension_gl_ECGLFilter_setParamFloatMulti
          (int *param_1,undefined4 param_2,int param_3,int param_4,int param_5,int param_6,
          int param_7)

{
  undefined4 uVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  undefined4 uVar5;
  int *piVar6;
  
  if (((param_3 == 0 || param_4 == 0) || (param_5 == 0 || param_6 == 0)) ||
     (4 < param_7 || param_7 < 1)) {
    uVar5 = 0xfffffc13;
  }
  else if ((**(int **)(DAT_00044f60 + 0x44e34) == 0) ||
          (piVar6 = *(int **)(DAT_00044f64 + 0x44e54), *piVar6 == 0)) {
    uVar5 = 0xfffff830;
  }
  else {
    uVar5 = (**(code **)(*param_1 + 0x194))(param_1,param_4);
    uVar1 = (**(code **)(*param_1 + 0x194))(param_1,param_3,*piVar6);
    iVar2 = (**(code **)(*param_1 + 0x2a4))(param_1,param_5,0);
    if (iVar2 == 0) {
      uVar5 = 0xfffffc16;
    }
    else {
      iVar3 = (**(code **)(*param_1 + 0x2ac))(param_1,param_6);
      if (iVar3 == 0) {
        uVar5 = 0xfffffc13;
      }
      else {
        iVar4 = (**(code **)(*param_1 + 0x2f4))(param_1,param_6,0);
        if (iVar4 == 0) {
          uVar5 = 0xfffff830;
        }
        else {
          uVar5 = ECGLSetParamFloatMulti(uVar1,uVar5,iVar2,iVar4,iVar3,param_7);
          (**(code **)(*param_1 + 0x314))(param_1,param_6,iVar4,2);
        }
      }
      (**(code **)(*param_1 + 0x2a8))(param_1,param_5,iVar2);
    }
  }
  return uVar5;
}

// ==== Java_com_adobe_ttpixel_extension_gl_ECGLFilter_setParamFloat @ 00044f68 ====


undefined4
Java_com_adobe_ttpixel_extension_gl_ECGLFilter_setParamFloat
          (int *param_1,undefined4 param_2,int param_3,int param_4,int param_5,undefined4 param_6)

{
  undefined4 uVar1;
  int iVar2;
  undefined4 uVar3;
  int *piVar4;
  
  if ((param_3 == 0 || param_4 == 0) || (param_5 == 0)) {
    uVar3 = 0xfffffc13;
  }
  else if ((**(int **)(DAT_00045060 + 0x44fac) == 0) ||
          (piVar4 = *(int **)(DAT_00045064 + 0x44fcc), *piVar4 == 0)) {
    uVar3 = 0xfffff830;
  }
  else {
    uVar3 = (**(code **)(*param_1 + 0x194))(param_1,param_4);
    uVar1 = (**(code **)(*param_1 + 0x194))(param_1,param_3,*piVar4);
    iVar2 = (**(code **)(*param_1 + 0x2a4))(param_1,param_5,0);
    if (iVar2 == 0) {
      uVar3 = 0xfffffc16;
    }
    else {
      uVar3 = ECGLSetParamFloat(uVar1,uVar3,iVar2,param_6);
      (**(code **)(*param_1 + 0x2a8))(param_1,param_5,iVar2);
    }
  }
  return uVar3;
}

// ==== Java_com_adobe_ttpixel_extension_gl_ECGLFilter_createFilter @ 00045068 ====


undefined4
Java_com_adobe_ttpixel_extension_gl_ECGLFilter_createFilter
          (int *param_1,undefined4 param_2,int param_3,int param_4,undefined4 param_5,
          undefined4 param_6,int param_7,int param_8)

{
  int iVar1;
  int iVar2;
  int iVar3;
  undefined4 uVar4;
  int local_38;
  int local_34;
  int local_2c [2];
  
  iVar2 = DAT_00045208;
  iVar1 = DAT_00045204;
  iVar3 = DAT_00045200 + 0x4508c;
  if (param_3 == 0 || param_4 == 0) {
    uVar4 = 0xfffffc13;
  }
  else if ((**(int **)(iVar3 + DAT_00045204) == 0) || (**(int **)(iVar3 + DAT_00045208) == 0)) {
    uVar4 = 0xfffff830;
  }
  else {
    if (param_7 == 0) {
      local_34 = param_7;
    }
    else {
      local_34 = (**(code **)(*param_1 + 0x2a4))(param_1,param_7,param_3 == 0 || param_4 == 0);
    }
    if (param_8 == 0) {
      local_38 = param_8;
    }
    else {
      local_38 = (**(code **)(*param_1 + 0x2a4))(param_1,param_8,0);
    }
    uVar4 = (**(code **)(*param_1 + 0x194))(param_1,param_4,**(undefined4 **)(iVar3 + iVar1));
    local_2c[0] = 0;
    uVar4 = ECGLCreateFilter(local_2c,uVar4,param_5,param_6,local_34,local_38);
    (**(code **)(*param_1 + 0x1b8))
              (param_1,param_3,**(undefined4 **)(iVar3 + iVar2),*param_1,local_2c[0],
               local_2c[0] >> 0x1f);
    if (local_38 != 0) {
      (**(code **)(*param_1 + 0x2a8))(param_1,param_8,local_38);
    }
    if (local_34 != 0) {
      (**(code **)(*param_1 + 0x2a8))(param_1,param_7,local_34);
    }
  }
  return uVar4;
}

// ==== Java_com_adobe_ttpixel_extension_gl_ECGLContext_getLastGLErrorCode @ 0004520c ====


undefined4
Java_com_adobe_ttpixel_extension_gl_ECGLContext_getLastGLErrorCode
          (int *param_1,undefined4 param_2,int param_3,int param_4)

{
  undefined4 uVar1;
  int *piVar2;
  undefined4 local_14;
  
  if (param_3 == 0 || param_4 == 0) {
    uVar1 = 0xfffffc13;
  }
  else if ((**(int **)(DAT_000452ac + 0x45244) == 0) ||
          (piVar2 = *(int **)(DAT_000452b0 + 0x45264), *piVar2 == 0)) {
    uVar1 = 0xfffff830;
  }
  else {
    uVar1 = (**(code **)(*param_1 + 0x194))(param_1,param_3);
    uVar1 = ECGLGetLastGLErrorCode(uVar1,&local_14);
    (**(code **)(*param_1 + 0x1b4))(param_1,param_4,*piVar2,local_14);
  }
  return uVar1;
}

// ==== Java_com_adobe_ttpixel_extension_gl_ECGLContext_destroyContext @ 000452b4 ====


undefined4
Java_com_adobe_ttpixel_extension_gl_ECGLContext_destroyContext
          (int *param_1,undefined4 param_2,int param_3)

{
  undefined4 uVar1;
  int *piVar2;
  
  if (param_3 == 0) {
    uVar1 = 0xfffffc13;
  }
  else {
    piVar2 = *(int **)(DAT_00045334 + 0x452e4);
    if (*piVar2 == 0) {
      uVar1 = 0xfffff830;
    }
    else {
      (**(code **)(*param_1 + 0x194))(param_1,param_3);
      uVar1 = ECGLDestroyContext();
      (**(code **)(*param_1 + 0x1b8))(param_1,param_3,*piVar2,*param_1,0,0);
    }
  }
  return uVar1;
}

// ==== Java_com_adobe_ttpixel_extension_gl_ECGLContext_createContext @ 00045338 ====


undefined4
Java_com_adobe_ttpixel_extension_gl_ECGLContext_createContext
          (int *param_1,undefined4 param_2,int param_3)

{
  undefined4 uVar1;
  int *piVar2;
  int local_1c [2];
  
  if (param_3 == 0) {
    uVar1 = 0xfffffc13;
  }
  else {
    piVar2 = *(int **)(DAT_000453ac + 0x45368);
    if (*piVar2 == 0) {
      uVar1 = 0xfffff830;
    }
    else {
      uVar1 = ECGLCreateContext(local_1c);
      (**(code **)(*param_1 + 0x1b8))
                (param_1,param_3,*piVar2,*param_1,local_1c[0],local_1c[0] >> 0x1f);
    }
  }
  return uVar1;
}

// ==== Java_com_adobe_ttpixel_extension_am_ECAlphaMatting_release @ 000453b0 ====


undefined4
Java_com_adobe_ttpixel_extension_am_ECAlphaMatting_release
          (int *param_1,undefined4 param_2,undefined4 param_3)

{
  undefined4 uVar1;
  
  if (**(int **)(DAT_000453ec + 0x453c4) != 0) {
    (**(code **)(*param_1 + 0x194))(param_1,param_3);
    uVar1 = ECAMRelease();
    return uVar1;
  }
  return 0xfffff830;
}

// ==== Java_com_adobe_ttpixel_extension_am_ECAlphaMatting_getResult @ 000453f0 ====


undefined4
Java_com_adobe_ttpixel_extension_am_ECAlphaMatting_getResult
          (int *param_1,undefined4 param_2,undefined4 param_3,int param_4)

{
  undefined4 uVar1;
  
  if (param_4 != 0) {
    param_4 = (**(code **)(*param_1 + 0x398))(param_1,param_4);
  }
  if (**(int **)(DAT_0004545c + 0x45424) != 0) {
    uVar1 = (**(code **)(*param_1 + 0x194))(param_1,param_3,**(int **)(DAT_0004545c + 0x45424));
    uVar1 = ECAMGetResult(uVar1,param_4);
    return uVar1;
  }
  return 0xfffff830;
}

// ==== Java_com_adobe_ttpixel_extension_am_ECAlphaMatting_getProgress @ 00045460 ====


undefined4
Java_com_adobe_ttpixel_extension_am_ECAlphaMatting_getProgress
          (int *param_1,undefined4 param_2,undefined4 param_3)

{
  undefined4 uVar1;
  
  if (**(int **)(DAT_00045498 + 0x45474) != 0) {
    (**(code **)(*param_1 + 0x194))(param_1,param_3);
    uVar1 = ECAMGetProgress();
    return uVar1;
  }
  return 0;
}

// ==== Java_com_adobe_ttpixel_extension_am_ECAlphaMatting_run @ 0004549c ====


undefined4
Java_com_adobe_ttpixel_extension_am_ECAlphaMatting_run
          (int *param_1,undefined4 param_2,undefined4 param_3)

{
  undefined4 uVar1;
  
  if (**(int **)(DAT_000454d8 + 0x454b0) != 0) {
    (**(code **)(*param_1 + 0x194))(param_1,param_3);
    uVar1 = ECAMRun();
    return uVar1;
  }
  return 0xfffff830;
}

// ==== Java_com_adobe_ttpixel_extension_am_ECAlphaMatting_init @ 000454dc ====


undefined4
Java_com_adobe_ttpixel_extension_am_ECAlphaMatting_init
          (int *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,undefined4 param_5,
          int param_6,int param_7)

{
  undefined4 uVar1;
  int *piVar2;
  undefined4 uVar3;
  int local_1c;
  
  if (param_7 == 0 || param_6 == 0) {
    uVar3 = 0xfffffc13;
  }
  else {
    uVar3 = (**(code **)(*param_1 + 0x398))();
    uVar1 = (**(code **)(*param_1 + 0x398))(param_1,param_7);
    piVar2 = *(int **)(DAT_000455c4 + 0x45548);
    if (*piVar2 == 0) {
      uVar3 = 0xfffff830;
    }
    else {
      local_1c = (**(code **)(*param_1 + 0x194))(param_1,param_3);
      uVar3 = ECAMInit(&local_1c,param_4,param_5,uVar3,uVar1);
      (**(code **)(*param_1 + 0x1b8))(param_1,param_3,*piVar2,*param_1,local_1c,local_1c >> 0x1f);
    }
  }
  return uVar3;
}

// ==== Java_com_adobe_ttpixel_extension_ss_ECScribbleSegment_release @ 000455c8 ====


undefined4
Java_com_adobe_ttpixel_extension_ss_ECScribbleSegment_release
          (int *param_1,undefined4 param_2,undefined4 param_3)

{
  undefined4 uVar1;
  
  if (**(int **)(DAT_00045604 + 0x455dc) != 0) {
    (**(code **)(*param_1 + 0x194))(param_1,param_3);
    uVar1 = ECSSRelease();
    return uVar1;
  }
  return 0xfffff830;
}

// ==== Java_com_adobe_ttpixel_extension_ss_ECScribbleSegment_getResult @ 00045608 ====


undefined4
Java_com_adobe_ttpixel_extension_ss_ECScribbleSegment_getResult
          (int *param_1,undefined4 param_2,undefined4 param_3)

{
  undefined4 uVar1;
  
  if (**(int **)(DAT_00045644 + 0x4561c) != 0) {
    (**(code **)(*param_1 + 0x194))(param_1,param_3);
    uVar1 = ECSSGetResult();
    return uVar1;
  }
  return 0xfffff830;
}

// ==== Java_com_adobe_ttpixel_extension_ss_ECScribbleSegment_getProgress @ 00045648 ====


undefined4
Java_com_adobe_ttpixel_extension_ss_ECScribbleSegment_getProgress
          (int *param_1,undefined4 param_2,undefined4 param_3)

{
  undefined4 uVar1;
  
  if (**(int **)(DAT_00045680 + 0x4565c) != 0) {
    (**(code **)(*param_1 + 0x194))(param_1,param_3);
    uVar1 = ECSSGetProgress();
    return uVar1;
  }
  return 0;
}

// ==== Java_com_adobe_ttpixel_extension_ss_ECScribbleSegment_run @ 00045684 ====


undefined4
Java_com_adobe_ttpixel_extension_ss_ECScribbleSegment_run
          (int *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,undefined4 param_5,
          int param_6,int param_7,int param_8)

{
  undefined4 uVar1;
  undefined4 uVar2;
  
  if ((param_7 == 0 || param_6 == 0) || (param_8 == 0)) {
    uVar1 = 0xfffffc13;
  }
  else {
    uVar1 = (**(code **)(*param_1 + 0x398))(param_1,param_6);
    (**(code **)(*param_1 + 0x398))(param_1,param_7);
    (**(code **)(*param_1 + 0x398))(param_1,param_8);
    if (**(int **)(DAT_00045770 + 0x4571c) != 0) {
      uVar2 = (**(code **)(*param_1 + 0x194))(param_1,param_3);
      uVar1 = ECSSRun(uVar2,param_4,param_5,uVar1);
      return uVar1;
    }
    uVar1 = 0xfffff830;
  }
  return uVar1;
}

// ==== Java_com_adobe_ttpixel_extension_ss_ECScribbleSegment_init @ 00045774 ====


undefined4
Java_com_adobe_ttpixel_extension_ss_ECScribbleSegment_init
          (int *param_1,undefined4 param_2,undefined4 param_3)

{
  int *piVar1;
  undefined4 uVar2;
  int local_1c [2];
  
  piVar1 = *(int **)(DAT_000457f4 + 0x45784);
  if (*piVar1 == 0) {
    uVar2 = 0xfffff830;
  }
  else {
    local_1c[0] = (**(code **)(*param_1 + 0x194))(param_1,param_3);
    uVar2 = ECSSInit(local_1c);
    (**(code **)(*param_1 + 0x1b8))
              (param_1,param_3,*piVar1,*param_1,local_1c[0],local_1c[0] >> 0x1f);
  }
  return uVar2;
}

// ==== Java_com_adobe_ttpixel_extension_TTPixelExtensionContextImpExp_startEncodeJPEG @ 00045958 ====


undefined4
Java_com_adobe_ttpixel_extension_TTPixelExtensionContextImpExp_startEncodeJPEG
          (int *param_1,undefined4 param_2,undefined4 param_3,int param_4,int param_5,char param_6,
          char param_7,uint param_8,undefined4 param_9,uchar *param_10,undefined4 param_11)

{
  int *piVar1;
  undefined4 uVar2;
  uint *puVar3;
  ECAsyncJPEGExporter *this;
  void *pvVar4;
  int iVar5;
  int iVar6;
  undefined4 uVar7;
  float extraout_s0;
  
  iVar5 = DAT_00045b48;
  iVar6 = DAT_00045b44 + 0x45970;
  piVar1 = (int *)(**(code **)(*param_1 + 0x194))
                            (param_1,param_3,**(undefined4 **)(iVar6 + DAT_00045b48));
  uVar2 = (**(code **)(*param_1 + 0x2a4))(param_1,param_11,0);
  puVar3 = (uint *)(**(code **)(*param_1 + 0x398))(param_1,param_9);
  if (puVar3 != (uint *)0x0) {
    if (piVar1 != (int *)0x0) {
      (**(code **)(*piVar1 + 4))(piVar1);
    }
    this = (ECAsyncJPEGExporter *)operator_new(0x370);
    if (param_6 != '\0') {
      param_6 = true;
    }
    if (param_7 != '\0') {
      param_7 = true;
    }
    uVar7 = uVar2;
    ECAsyncJPEGExporter::ECAsyncJPEGExporter
              (this,param_4,param_5,(bool)param_6,(bool)param_7,param_8,puVar3,extraout_s0,param_10)
    ;
    (**(code **)(*param_1 + 0x1b8))
              (param_1,param_3,**(undefined4 **)(iVar6 + iVar5),*param_1,this,(int)this >> 0x1f,
               puVar3,param_10,uVar7);
    if (this != (ECAsyncJPEGExporter *)0x0) {
      pvVar4 = (void *)(**(code **)(*param_1 + 0x54))(param_1,param_3);
      ECAsyncExporter::RegisterCallbacks
                ((ECAsyncExporter *)this,(_func_bool_int_void_ptr *)(DAT_00045b4c + 0x45a8c),
                 (_func_void_int_void_ptr *)(DAT_00045b50 + 0x45a90),pvVar4);
      iVar5 = ECAsyncWorker::Start((ECAsyncWorker *)this);
      if (iVar5 == 0) {
        (**(code **)(*param_1 + 0x58))(param_1,pvVar4);
        uVar7 = 0;
      }
      else {
        uVar7 = 1;
      }
      goto LAB_00045ab0;
    }
  }
  uVar7 = 0;
LAB_00045ab0:
  (**(code **)(*param_1 + 0x2a8))(param_1,param_11,uVar2);
  return uVar7;
}

// ==== Java_com_adobe_ttpixel_extension_TTPixelExtensionContextImpExp_premultiplyData @ 00045b54 ====


void Java_com_adobe_ttpixel_extension_TTPixelExtensionContextImpExp_premultiplyData
               (int *param_1,undefined4 param_2,int param_3,int param_4,uint param_5,
               undefined4 param_6)

{
  uint *puVar1;
  
  puVar1 = (uint *)(**(code **)(*param_1 + 0x398))(param_1,param_6);
  ECAsyncBitmapExporter::PremultiplyData(param_3,param_4,param_5,puVar1);
  return;
}

// ==== Java_com_adobe_ttpixel_extension_TTPixelExtensionContextImpExp_unPremultiplyData @ 00045b8c ====


void Java_com_adobe_ttpixel_extension_TTPixelExtensionContextImpExp_unPremultiplyData
               (int *param_1,undefined4 param_2,int param_3,int param_4,uint param_5,
               undefined4 param_6)

{
  uint *puVar1;
  
  puVar1 = (uint *)(**(code **)(*param_1 + 0x398))(param_1,param_6);
  ECAsyncBitmapExporter::UnPremultiplyData(param_3,param_4,param_5,puVar1);
  return;
}

// ==== Java_com_adobe_ttpixel_extension_TTPixelExtensionContextImpExp_isPossiblyPremultipliedData @ 00045bc4 ====


void Java_com_adobe_ttpixel_extension_TTPixelExtensionContextImpExp_isPossiblyPremultipliedData
               (int *param_1,undefined4 param_2,int param_3,int param_4,uint param_5,
               undefined4 param_6)

{
  uint *puVar1;
  
  puVar1 = (uint *)(**(code **)(*param_1 + 0x398))(param_1,param_6);
  ECAsyncBitmapExporter::IsPossiblyPremultipliedData(param_3,param_4,param_5,puVar1);
  return;
}

// ==== Java_com_adobe_ttpixel_extension_TTPixelExtensionContextImpExp_hasFinishedEncoding @ 00045bfc ====


int Java_com_adobe_ttpixel_extension_TTPixelExtensionContextImpExp_hasFinishedEncoding
              (int *param_1,undefined4 param_2,undefined4 param_3)

{
  int iVar1;
  
  iVar1 = (**(code **)(*param_1 + 0x194))(param_1,param_3,**(undefined4 **)(DAT_00045c34 + 0x45c10))
  ;
  if (iVar1 == 0) {
    return 0;
  }
  iVar1 = *(int *)(iVar1 + 8);
  if (iVar1 != 0) {
    iVar1 = 1;
  }
  return iVar1;
}

// ==== Java_com_adobe_ttpixel_extension_TTPixelExtensionContextImpExp_getEncodedDataSize @ 00045c38 ====


undefined4
Java_com_adobe_ttpixel_extension_TTPixelExtensionContextImpExp_getEncodedDataSize
          (int *param_1,undefined4 param_2,undefined4 param_3)

{
  int iVar1;
  
  iVar1 = (**(code **)(*param_1 + 0x194))(param_1,param_3,**(undefined4 **)(DAT_00045c78 + 0x45c4c))
  ;
  if ((iVar1 != 0) && (*(int *)(iVar1 + 8) != 0)) {
    return *(undefined4 *)(iVar1 + 0x20);
  }
  return 0;
}

// ==== Java_com_adobe_ttpixel_extension_TTPixelExtensionContextImpExp_waitFinishedEncoding @ 00045c7c ====


void * Java_com_adobe_ttpixel_extension_TTPixelExtensionContextImpExp_waitFinishedEncoding
                 (int *param_1,undefined4 param_2,undefined4 param_3)

{
  ECAsyncWorker *this;
  void *local_c [3];
  
  this = (ECAsyncWorker *)
         (**(code **)(*param_1 + 0x194))(param_1,param_3,**(undefined4 **)(DAT_00045cc8 + 0x45c90));
  if (this == (ECAsyncWorker *)0x0) {
    local_c[0] = (void *)0x0;
  }
  else {
    local_c[0] = (void *)0x0;
    ECAsyncWorker::WaitFinish(this,local_c);
  }
  return local_c[0];
}

// ==== Java_com_adobe_ttpixel_extension_TTPixelExtensionContextImpExp_clearEncodedData @ 00045ccc ====


undefined4
Java_com_adobe_ttpixel_extension_TTPixelExtensionContextImpExp_clearEncodedData
          (int *param_1,undefined4 param_2,undefined4 param_3)

{
  ECAsyncExporter *this;
  
  this = (ECAsyncExporter *)
         (**(code **)(*param_1 + 0x194))(param_1,param_3,**(undefined4 **)(DAT_00045d14 + 0x45ce0));
  if ((this != (ECAsyncExporter *)0x0) && (*(int *)(this + 8) != 0)) {
    ECAsyncExporter::DestBufferFree(this);
    return 1;
  }
  return 0;
}

// ==== Java_com_adobe_ttpixel_extension_TTPixelExtensionContextImpExp_getEncodingProgress @ 00045d18 ====


void Java_com_adobe_ttpixel_extension_TTPixelExtensionContextImpExp_getEncodingProgress
               (int *param_1,undefined4 param_2,undefined4 param_3)

{
  int *piVar1;
  
  piVar1 = (int *)(**(code **)(*param_1 + 0x194))
                            (param_1,param_3,**(undefined4 **)(DAT_00045d54 + 0x45d2c));
  if (piVar1 != (int *)0x0) {
    (**(code **)(*piVar1 + 8))(piVar1);
    return;
  }
  return;
}

// ==== Java_com_adobe_ttpixel_extension_TTPixelExtensionContextImpExp_requestCancel @ 00045d58 ====


void Java_com_adobe_ttpixel_extension_TTPixelExtensionContextImpExp_requestCancel
               (int *param_1,undefined4 param_2,undefined4 param_3)

{
  ECAsyncWorker *this;
  int iVar1;
  
  this = (ECAsyncWorker *)
         (**(code **)(*param_1 + 0x194))(param_1,param_3,**(undefined4 **)(DAT_00045da8 + 0x45d6c));
  if (this == (ECAsyncWorker *)0x0) {
    return;
  }
  iVar1 = pthread_equal(**(pthread_t **)(DAT_00045dac + 0x45d90),*(pthread_t *)(this + 0xc));
  if (iVar1 != 0) {
    return;
  }
  ECAsyncWorker::RequestCancel(this);
  return;
}

// ==== Java_com_adobe_ttpixel_extension_TTPixelExtensionContextImpExp_getEncodedData @ 00045db0 ====


undefined4
Java_com_adobe_ttpixel_extension_TTPixelExtensionContextImpExp_getEncodedData
          (int *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  int iVar1;
  void *__dest;
  size_t __n;
  undefined8 uVar2;
  
  iVar1 = (**(code **)(*param_1 + 0x194))(param_1,param_3,**(undefined4 **)(DAT_00045e60 + 0x45dd0))
  ;
  if ((((iVar1 != 0) && (*(int *)(iVar1 + 8) != 0)) && (__n = *(size_t *)(iVar1 + 0x20), __n != 0))
     && (((uVar2 = (**(code **)(*param_1 + 0x39c))(param_1,param_4), __n == (size_t)uVar2 &&
          ((int)((ulonglong)uVar2 >> 0x20) == 0)) &&
         ((__dest = (void *)(**(code **)(*param_1 + 0x398))(param_1,param_4), __dest != (void *)0x0
          && (*(void **)(iVar1 + 0x1c) != (void *)0x0)))))) {
    memmove(__dest,*(void **)(iVar1 + 0x1c),__n);
    return 1;
  }
  return 0;
}

// ==== Java_com_adobe_ttpixel_extension_TTPixelExtensionContextImpExp_startEncodePNG @ 00045f58 ====


undefined4
Java_com_adobe_ttpixel_extension_TTPixelExtensionContextImpExp_startEncodePNG
          (int *param_1,undefined4 param_2,undefined4 param_3,int param_4,int param_5,char param_6,
          char param_7,uint param_8,undefined4 param_9)

{
  int *piVar1;
  uint *puVar2;
  ECAsyncPNGExporter *this;
  void *pvVar3;
  int iVar4;
  int iVar5;
  
  iVar4 = DAT_00046108;
  iVar5 = DAT_00046104 + 0x45f70;
  piVar1 = (int *)(**(code **)(*param_1 + 0x194))
                            (param_1,param_3,**(undefined4 **)(iVar5 + DAT_00046108));
  puVar2 = (uint *)(**(code **)(*param_1 + 0x398))(param_1,param_9);
  if (puVar2 != (uint *)0x0) {
    if (piVar1 != (int *)0x0) {
      (**(code **)(*piVar1 + 4))(piVar1);
    }
    this = (ECAsyncPNGExporter *)operator_new(0x50);
    if (param_6 != '\0') {
      param_6 = true;
    }
    if (param_7 != '\0') {
      param_7 = true;
    }
    ECAsyncPNGExporter::ECAsyncPNGExporter
              (this,param_4,param_5,(bool)param_6,(bool)param_7,param_8,puVar2);
    (**(code **)(*param_1 + 0x1b8))
              (param_1,param_3,**(undefined4 **)(iVar5 + iVar4),*param_1,this,(int)this >> 0x1f);
    if (this != (ECAsyncPNGExporter *)0x0) {
      pvVar3 = (void *)(**(code **)(*param_1 + 0x54))(param_1,param_3);
      ECAsyncExporter::RegisterCallbacks
                ((ECAsyncExporter *)this,(_func_bool_int_void_ptr *)(DAT_0004610c + 0x46064),
                 (_func_void_int_void_ptr *)(DAT_00046110 + 0x46068),pvVar3);
      iVar4 = ECAsyncWorker::Start((ECAsyncWorker *)this);
      if (iVar4 == 0) {
        (**(code **)(*param_1 + 0x58))(param_1,pvVar3);
        return 0;
      }
      return 1;
    }
  }
  return 0;
}

// ==== Java_com_adobe_ttpixel_extension_TTPixelExtensionContextImpExp_startEncodeZLib @ 00046114 ====


undefined4
Java_com_adobe_ttpixel_extension_TTPixelExtensionContextImpExp_startEncodeZLib
          (int *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  int *piVar1;
  uchar *puVar2;
  uint uVar3;
  ECAsyncZLibExporter *this;
  void *pvVar4;
  int iVar5;
  int iVar6;
  
  iVar5 = DAT_000462ac;
  iVar6 = DAT_000462a8 + 0x4612c;
  piVar1 = (int *)(**(code **)(*param_1 + 0x194))
                            (param_1,param_3,**(undefined4 **)(iVar6 + DAT_000462ac));
  puVar2 = (uchar *)(**(code **)(*param_1 + 0x398))(param_1,param_4);
  if (puVar2 != (uchar *)0x0) {
    if (piVar1 != (int *)0x0) {
      (**(code **)(*piVar1 + 4))(piVar1);
    }
    uVar3 = (**(code **)(*param_1 + 0x39c))(param_1,param_4);
    this = (ECAsyncZLibExporter *)operator_new(0x38);
    ECAsyncZLibExporter::ECAsyncZLibExporter(this,puVar2,uVar3);
    (**(code **)(*param_1 + 0x1b8))
              (param_1,param_3,**(undefined4 **)(iVar6 + iVar5),*param_1,this,(int)this >> 0x1f);
    if (this != (ECAsyncZLibExporter *)0x0) {
      pvVar4 = (void *)(**(code **)(*param_1 + 0x54))(param_1,param_3);
      ECAsyncExporter::RegisterCallbacks
                ((ECAsyncExporter *)this,(_func_bool_int_void_ptr *)(DAT_000462b0 + 0x46208),
                 (_func_void_int_void_ptr *)(DAT_000462b4 + 0x4620c),pvVar4);
      iVar5 = ECAsyncWorker::Start((ECAsyncWorker *)this);
      if (iVar5 == 0) {
        (**(code **)(*param_1 + 0x58))(param_1,pvVar4);
        return 0;
      }
      return 1;
    }
  }
  return 0;
}

// ==== Java_com_adobe_ttpixel_extension_TTPixelExtensionContextImpExp_startEncodeLz4 @ 000462b8 ====


undefined4
Java_com_adobe_ttpixel_extension_TTPixelExtensionContextImpExp_startEncodeLz4
          (int *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  int *piVar1;
  uchar *puVar2;
  uint uVar3;
  ECAsyncLz4Exporter *this;
  void *pvVar4;
  int iVar5;
  int iVar6;
  
  iVar5 = DAT_00046450;
  iVar6 = DAT_0004644c + 0x462d0;
  piVar1 = (int *)(**(code **)(*param_1 + 0x194))
                            (param_1,param_3,**(undefined4 **)(iVar6 + DAT_00046450));
  puVar2 = (uchar *)(**(code **)(*param_1 + 0x398))(param_1,param_4);
  if (puVar2 != (uchar *)0x0) {
    if (piVar1 != (int *)0x0) {
      (**(code **)(*piVar1 + 4))(piVar1);
    }
    uVar3 = (**(code **)(*param_1 + 0x39c))(param_1,param_4);
    this = (ECAsyncLz4Exporter *)operator_new(0x38);
    ECAsyncLz4Exporter::ECAsyncLz4Exporter(this,puVar2,uVar3);
    (**(code **)(*param_1 + 0x1b8))
              (param_1,param_3,**(undefined4 **)(iVar6 + iVar5),*param_1,this,(int)this >> 0x1f);
    if (this != (ECAsyncLz4Exporter *)0x0) {
      pvVar4 = (void *)(**(code **)(*param_1 + 0x54))(param_1,param_3);
      ECAsyncExporter::RegisterCallbacks
                ((ECAsyncExporter *)this,(_func_bool_int_void_ptr *)(DAT_00046454 + 0x463ac),
                 (_func_void_int_void_ptr *)(DAT_00046458 + 0x463b0),pvVar4);
      iVar5 = ECAsyncWorker::Start((ECAsyncWorker *)this);
      if (iVar5 == 0) {
        (**(code **)(*param_1 + 0x58))(param_1,pvVar4);
        return 0;
      }
      return 1;
    }
  }
  return 0;
}

// ==== Java_com_adobe_ttpixel_extension_httpd_Httpd_modifyPasswdFile @ 0004645c ====


undefined4
Java_com_adobe_ttpixel_extension_httpd_Httpd_modifyPasswdFile
          (int *param_1,undefined4 param_2,int param_3,int param_4,int param_5,int param_6)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  undefined4 uVar5;
  
  if (param_3 == 0) {
    iVar1 = 0;
  }
  else {
    iVar1 = (**(code **)(*param_1 + 0x2a4))(param_1,param_3,0);
  }
  if (param_4 == 0) {
    iVar2 = 0;
  }
  else {
    iVar2 = (**(code **)(*param_1 + 0x2a4))(param_1,param_4,0);
  }
  if (param_5 == 0) {
    iVar3 = 0;
  }
  else {
    iVar3 = (**(code **)(*param_1 + 0x2a4))(param_1,param_5,0);
  }
  if (param_6 == 0) {
    iVar4 = 0;
  }
  else {
    iVar4 = (**(code **)(*param_1 + 0x2a4))(param_1,param_6,0);
  }
  uVar5 = ECHttpdModifyPasswdFile(iVar1,iVar2,iVar3,iVar4);
  if (iVar1 != 0) {
    (**(code **)(*param_1 + 0x2a8))(param_1,param_3,iVar1);
  }
  if (iVar2 != 0) {
    (**(code **)(*param_1 + 0x2a8))(param_1,param_4,iVar2);
  }
  if (iVar3 != 0) {
    (**(code **)(*param_1 + 0x2a8))(param_1,param_5,iVar3);
  }
  if (iVar4 != 0) {
    (**(code **)(*param_1 + 0x2a8))(param_1,param_6,iVar4);
  }
  return uVar5;
}

// ==== Java_com_adobe_ttpixel_extension_httpd_Httpd_stop @ 000465b4 ====


void Java_com_adobe_ttpixel_extension_httpd_Httpd_stop(void)

{
  int *piVar1;
  
  piVar1 = (int *)(DAT_0004e05c + 0x4e044);
  if (*piVar1 == 0) {
    return;
  }
  mg_stop();
  *piVar1 = 0;
  return;
}

// ==== Java_com_adobe_ttpixel_extension_httpd_Httpd_start @ 000465b8 ====


undefined4
Java_com_adobe_ttpixel_extension_httpd_Httpd_start(int *param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  void *__ptr;
  int iVar2;
  undefined4 uVar3;
  void *pvVar4;
  int iVar5;
  void *pvVar6;
  int iVar7;
  undefined4 local_2c;
  
  if ((param_3 == 0) || (iVar1 = (**(code **)(*param_1 + 0x2ac))(param_1,param_3), iVar1 < 1)) {
    pvVar6 = (void *)0x0;
    local_2c = (void *)0x0;
    pvVar4 = (void *)0x0;
    __ptr = (void *)0x0;
LAB_00046704:
    uVar3 = ECHttpdStart(__ptr);
    if (pvVar6 == (void *)0x0) goto LAB_000466d8;
  }
  else {
    __ptr = malloc((iVar1 + 1) * 4);
    local_2c = malloc(iVar1 * 4);
    pvVar4 = __ptr;
    if (__ptr != (void *)0x0) {
      pvVar4 = (void *)0x1;
    }
    pvVar6 = local_2c;
    if (local_2c != (void *)0x0) {
      pvVar6 = (void *)0x1;
    }
    if (((uint)pvVar6 & (uint)pvVar4) == 0) goto LAB_00046704;
    iVar5 = 0;
    iVar7 = 0;
    do {
      iVar2 = (**(code **)(*param_1 + 0x2b4))(param_1,param_3,iVar7);
      *(int *)((int)local_2c + iVar5) = iVar2;
      if (iVar2 == 0) {
        uVar3 = 0;
      }
      else {
        uVar3 = (**(code **)(*param_1 + 0x2a4))(param_1,iVar2);
      }
      iVar7 = iVar7 + 1;
      *(undefined4 *)((int)__ptr + iVar5) = uVar3;
      iVar5 = iVar5 + 4;
    } while (iVar7 != iVar1);
    iVar5 = 0;
    *(undefined4 *)((int)__ptr + iVar1 * 4) = 0;
    iVar7 = 0;
    uVar3 = ECHttpdStart(__ptr);
    do {
      iVar7 = iVar7 + 1;
      if (*(int *)((int)__ptr + iVar5) != 0) {
        (**(code **)(*param_1 + 0x2a8))(param_1,*(undefined4 *)((int)local_2c + iVar5));
      }
      iVar5 = iVar5 + 4;
    } while (iVar7 != iVar1);
    pvVar4 = (void *)0x1;
  }
  free(local_2c);
LAB_000466d8:
  if (pvVar4 != (void *)0x0) {
    free(__ptr);
  }
  return uVar3;
}

// ==== Java_com_adobe_ttpixel_extension_TTPixelExtension_initIDs @ 000468b0 ====


void Java_com_adobe_ttpixel_extension_TTPixelExtension_initIDs(int *param_1)

{
  undefined4 uVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  
  uVar1 = (**(code **)(*param_1 + 0x18))(param_1,DAT_00046aa0 + 0x468c0);
  iVar6 = DAT_00046aa4 + 0x468dc;
  iVar5 = DAT_00046aa8 + 0x468e4;
  iVar4 = DAT_00046aac + 0x468f4;
  uVar1 = (**(code **)(*param_1 + 0x178))(param_1,uVar1,iVar6,iVar5);
  iVar3 = *param_1;
  iVar2 = DAT_00046ab4 + 0x46918;
  **(undefined4 **)(iVar4 + DAT_00046ab0) = uVar1;
  uVar1 = (**(code **)(iVar3 + 0x18))(param_1,iVar2);
  uVar1 = (**(code **)(*param_1 + 0x178))(param_1,uVar1,iVar6,iVar5);
  iVar3 = *param_1;
  iVar2 = DAT_00046abc + 0x46958;
  **(undefined4 **)(iVar4 + DAT_00046ab8) = uVar1;
  uVar1 = (**(code **)(iVar3 + 0x18))(param_1,iVar2);
  uVar1 = (**(code **)(*param_1 + 0x178))(param_1,uVar1,iVar6,iVar5);
  iVar3 = *param_1;
  iVar2 = DAT_00046ac4 + 0x46998;
  **(undefined4 **)(iVar4 + DAT_00046ac0) = uVar1;
  uVar1 = (**(code **)(iVar3 + 0x18))(param_1,iVar2);
  uVar1 = (**(code **)(*param_1 + 0x178))(param_1,uVar1,iVar6,iVar5);
  iVar3 = *param_1;
  iVar2 = DAT_00046acc + 0x469d8;
  **(undefined4 **)(iVar4 + DAT_00046ac8) = uVar1;
  uVar1 = (**(code **)(iVar3 + 0x18))(param_1,iVar2);
  uVar1 = (**(code **)(*param_1 + 0x178))
                    (param_1,uVar1,DAT_00046ad0 + 0x469f8,DAT_00046ad4 + 0x469fc);
  iVar3 = *param_1;
  iVar2 = DAT_00046adc + 0x46a20;
  **(undefined4 **)(iVar4 + DAT_00046ad8) = uVar1;
  uVar1 = (**(code **)(iVar3 + 0x18))(param_1,iVar2);
  uVar1 = (**(code **)(*param_1 + 0x178))(param_1,uVar1,DAT_00046ae0 + 0x46a40,iVar5);
  iVar3 = *param_1;
  iVar2 = DAT_00046ae8 + 0x46a64;
  **(undefined4 **)(iVar4 + DAT_00046ae4) = uVar1;
  uVar1 = (**(code **)(iVar3 + 0x18))(param_1,iVar2);
  uVar1 = (**(code **)(*param_1 + 0x178))(param_1,uVar1,DAT_00046aec + 0x46a84,iVar5);
  **(undefined4 **)(iVar4 + DAT_00046af0) = uVar1;
  return;
}

// ==== Java_com_adobe_ttpixel_extension_ByteBufferFactory_copySwapColorChannels @ 00046b94 ====


undefined4
Java_com_adobe_ttpixel_extension_ByteBufferFactory_copySwapColorChannels
          (int *param_1,undefined4 param_2,int param_3,int param_4)

{
  uint uVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  
  if (param_4 != 0 && param_3 != 0) {
    uVar1 = (**(code **)(*param_1 + 0x39c))(param_1,param_3);
    uVar2 = (**(code **)(*param_1 + 0x39c))(param_1,param_4);
    if (((uVar2 <= uVar1) && ((uVar1 & 3) == 0)) && ((uVar2 & 3) == 0)) {
      iVar3 = (**(code **)(*param_1 + 0x398))(param_1,param_3);
      iVar4 = (**(code **)(*param_1 + 0x398))(param_1,param_4);
      if (iVar4 != 0 && iVar3 != 0) {
        iVar5 = 0;
        for (uVar2 = uVar2 >> 2; uVar2 != 0; uVar2 = uVar2 - 1) {
          uVar1 = *(uint *)(iVar4 + iVar5);
          *(uint *)(iVar3 + iVar5) =
               (uVar1 & 0xff) << 0x10 | (uVar1 << 8) >> 0x18 | uVar1 & 0xff00ff00;
          iVar5 = iVar5 + 4;
        }
        return 1;
      }
    }
  }
  return 0;
}

// ==== Java_com_adobe_ttpixel_extension_ByteBufferFactory_flipImageHor @ 00046c74 ====


undefined4
Java_com_adobe_ttpixel_extension_ByteBufferFactory_flipImageHor
          (int *param_1,undefined4 param_2,int param_3,int param_4,int param_5,int param_6)

{
  int iVar1;
  undefined4 *puVar2;
  undefined4 uVar3;
  undefined4 *puVar4;
  undefined4 *puVar5;
  undefined4 *puVar6;
  undefined4 *puVar7;
  undefined4 *puVar8;
  bool bVar9;
  
  bVar9 = param_3 < 0;
  if (param_3 != 0) {
    bVar9 = param_4 < 0;
  }
  if ((param_3 != 0 && param_4 != 0) && !bVar9) {
    bVar9 = param_6 == param_4;
    iVar1 = param_6 - param_4;
    if (param_6 >= param_4) {
      bVar9 = param_5 == 0;
      iVar1 = param_5;
    }
    if (((!bVar9 && iVar1 < 0 == (param_6 < param_4 && SBORROW4(param_6,param_4))) &&
        (iVar1 = (**(code **)(*param_1 + 0x39c))(param_1,param_3),
        param_6 * param_5 * 4 - iVar1 == 0)) &&
       (puVar2 = (undefined4 *)(**(code **)(*param_1 + 0x398))(param_1,param_3),
       puVar2 != (undefined4 *)0x0)) {
      puVar8 = puVar2 + param_4 + -1;
      while( true ) {
        puVar4 = puVar2;
        puVar6 = puVar8;
        if (puVar2 < puVar8) {
          do {
            uVar3 = *puVar4;
            puVar5 = puVar4 + 1;
            *puVar4 = *puVar6;
            puVar7 = puVar6 + -1;
            *puVar6 = uVar3;
            puVar4 = puVar5;
            puVar6 = puVar7;
          } while (puVar5 < puVar7);
        }
        param_5 = param_5 + -1;
        puVar8 = puVar8 + param_6;
        if (param_5 == 0) break;
        puVar2 = puVar2 + param_6;
      }
    }
  }
  return 0;
}

// ==== Java_com_adobe_ttpixel_extension_ByteBufferFactory_copyDirect @ 00046d30 ====


undefined4
Java_com_adobe_ttpixel_extension_ByteBufferFactory_copyDirect
          (int *param_1,undefined4 param_2,int param_3,int param_4)

{
  uint uVar1;
  uint __n;
  void *__dest;
  void *__src;
  
  if (param_4 != 0 && param_3 != 0) {
    uVar1 = (**(code **)(*param_1 + 0x39c))(param_1,param_3);
    __n = (**(code **)(*param_1 + 0x39c))(param_1,param_4);
    if (__n <= uVar1) {
      __dest = (void *)(**(code **)(*param_1 + 0x398))(param_1,param_3);
      __src = (void *)(**(code **)(*param_1 + 0x398))(param_1,param_4);
      if (__src != (void *)0x0 && __dest != (void *)0x0) {
        memcpy(__dest,__src,__n);
        return 1;
      }
    }
  }
  return 0;
}

// ==== Java_com_adobe_ttpixel_extension_ByteBufferFactory_freeDirect @ 00046dd8 ====


void Java_com_adobe_ttpixel_extension_ByteBufferFactory_freeDirect
               (int *param_1,undefined4 param_2,int param_3)

{
  void *__ptr;
  
  if (param_3 == 0) {
    return;
  }
  __ptr = (void *)(**(code **)(*param_1 + 0x398))();
  if (__ptr == (void *)0x0) {
    return;
  }
  free(__ptr);
  return;
}

// ==== Java_com_adobe_ttpixel_extension_ByteBufferFactory_allocateDirect @ 00046e00 ====


void Java_com_adobe_ttpixel_extension_ByteBufferFactory_allocateDirect
               (int *param_1,undefined4 param_2,size_t param_3)

{
  int iVar1;
  void *pvVar2;
  undefined4 uVar3;
  int iVar4;
  char acStack_64 [64];
  int local_24;
  
  iVar1 = DAT_00046ea4;
  iVar4 = DAT_00046ea0 + 0x46e18;
  local_24 = **(int **)(iVar4 + DAT_00046ea4);
  pvVar2 = malloc(param_3);
  if (pvVar2 == (void *)0x0) {
    sprintf(acStack_64,(char *)(DAT_00046ea8 + 0x46e88),param_3);
    throwOutOfMemoryError(param_1,acStack_64);
    uVar3 = 0;
  }
  else {
    uVar3 = (**(code **)(*param_1 + 0x394))(param_1,pvVar2,param_3,(int)param_3 >> 0x1f);
  }
  if (local_24 == **(int **)(iVar4 + iVar1)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail(uVar3);
}

// ==== Java_com_adobe_ttpixel_extension_utils_ECUtils_getTimestamp @ 00046eac ====


void Java_com_adobe_ttpixel_extension_utils_ECUtils_getTimestamp(void)

{
  ECUtilsTimerGetTimestamp();
  __aeabi_ul2d();
  return;
}

// ==== Java_com_adobe_ttpixel_extension_utils_ECUtils_copyBitmapData @ 00046ebc ====


void Java_com_adobe_ttpixel_extension_utils_ECUtils_copyBitmapData
               (int *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,
               undefined4 param_5,undefined4 param_6)

{
  undefined4 uVar1;
  undefined4 uVar2;
  
  uVar1 = (**(code **)(*param_1 + 0x398))(param_1,param_3);
  uVar2 = (**(code **)(*param_1 + 0x398))(param_1,param_5);
  ECCopyBitmapData(uVar1,param_4,uVar2,param_6);
  return;
}

// ==== Java_com_adobe_ttpixel_extension_utils_ECUtils_alphaBlend @ 00046f20 ====


void Java_com_adobe_ttpixel_extension_utils_ECUtils_alphaBlend
               (int *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,
               undefined4 param_5,undefined4 param_6,undefined4 param_7,undefined4 param_8,
               undefined4 param_9,undefined4 param_10,undefined4 param_11,undefined4 param_12,
               undefined4 param_13,undefined4 param_14,undefined4 param_15,undefined4 param_16)

{
  undefined4 uVar1;
  
  uVar1 = (**(code **)(*param_1 + 0x398))
                    (param_1,param_5,param_16,*param_1,param_8,param_3,param_10,param_12,param_13,
                     param_14,param_15,param_16);
  (**(code **)(*param_1 + 0x398))(param_1,param_7);
  (**(code **)(*param_1 + 0x398))(param_1,param_9);
  (**(code **)(*param_1 + 0x398))(param_1,param_11);
  ECAlphaBlend(param_3,param_4,uVar1,param_6);
  return;
}

// ==== Java_com_adobe_ttpixel_extension_utils_ECUtils_bitmapDataCopy @ 00047048 ====


undefined4
Java_com_adobe_ttpixel_extension_utils_ECUtils_bitmapDataCopy
          (int *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,undefined4 param_5,
          undefined4 param_6)

{
  int iVar1;
  int iVar2;
  undefined4 uVar3;
  
  iVar1 = (**(code **)(*param_1 + 0x398))(param_1,param_3);
  iVar2 = (**(code **)(*param_1 + 0x398))(param_1,param_5);
  if (iVar2 != 0 && iVar1 != 0) {
    uVar3 = ECBitmapDataCopy(iVar1,param_4,iVar2,param_6);
    return uVar3;
  }
  return 0;
}

// ==== Java_com_adobe_ttpixel_extension_utils_ECUtils_bitmapDataResample @ 00047108 ====


undefined4
Java_com_adobe_ttpixel_extension_utils_ECUtils_bitmapDataResample
          (int *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,undefined4 param_5,
          undefined4 param_6)

{
  int iVar1;
  int iVar2;
  undefined4 uVar3;
  
  iVar1 = (**(code **)(*param_1 + 0x398))(param_1,param_3);
  iVar2 = (**(code **)(*param_1 + 0x398))(param_1,param_5);
  if (iVar2 != 0 && iVar1 != 0) {
    uVar3 = ECBitmapDataResample(iVar1,param_4,iVar2,param_6);
    return uVar3;
  }
  return 0;
}

// ==== Java_com_adobe_ttpixel_extension_utils_ECUtils_bitmapFileResample @ 000471e8 ====


undefined4
Java_com_adobe_ttpixel_extension_utils_ECUtils_bitmapFileResample
          (int *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,undefined4 param_5,
          undefined4 param_6,undefined4 param_7,undefined4 param_8,undefined4 param_9,
          undefined4 param_10,undefined4 param_11,undefined4 param_12,undefined4 param_13)

{
  int iVar1;
  int iVar2;
  undefined4 uVar3;
  
  iVar1 = (**(code **)(*param_1 + 0x2a4))(param_1,param_3,0);
  if (iVar1 == 0) {
    uVar3 = 0;
  }
  else {
    iVar2 = (**(code **)(*param_1 + 0x398))(param_1,param_4);
    if ((iVar2 == 0) ||
       (iVar2 = ECBitmapFileResample
                          (iVar1,iVar2,param_5,param_10,param_11,param_12,param_13,param_6,param_7,
                           param_8,param_9), iVar2 == 0)) {
      uVar3 = 0;
    }
    else {
      uVar3 = 1;
    }
    (**(code **)(*param_1 + 0x2a8))(param_1,param_3,iVar1);
  }
  return uVar3;
}

// ==== Java_com_adobe_ttpixel_extension_utils_ECUtils_getOnlineCPUCount @ 000472b8 ====


long Java_com_adobe_ttpixel_extension_utils_ECUtils_getOnlineCPUCount(void)

{
  long lVar1;
  
  lVar1 = sysconf(0x61);
  if (lVar1 < 1) {
    lVar1 = 1;
  }
  return lVar1;
}

// ==== Java_com_adobe_ttpixel_extension_utils_ECUtils_getInstalledCPUCount @ 000472bc ====


long Java_com_adobe_ttpixel_extension_utils_ECUtils_getInstalledCPUCount(void)

{
  long lVar1;
  
  lVar1 = sysconf(0x60);
  if (lVar1 < 1) {
    lVar1 = 1;
  }
  return lVar1;
}

// ==== Java_com_adobe_ttpixel_extension_utils_ECUtils_bitmapFileRead @ 000472c0 ====


undefined4
Java_com_adobe_ttpixel_extension_utils_ECUtils_bitmapFileRead
          (int *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,undefined4 param_5,
          undefined4 param_6,undefined4 param_7,undefined4 param_8,undefined4 param_9,
          undefined4 param_10,undefined4 param_11)

{
  int iVar1;
  int iVar2;
  undefined4 uVar3;
  
  iVar1 = (**(code **)(*param_1 + 0x2a4))(param_1,param_3,0);
  if (iVar1 == 0) {
    uVar3 = 0;
  }
  else {
    iVar2 = (**(code **)(*param_1 + 0x398))(param_1,param_4);
    if ((iVar2 == 0) ||
       (iVar2 = ECBitmapFileRead(iVar1,iVar2,param_5,param_6,param_7,param_8,param_9,param_10,
                                 param_11), iVar2 == 0)) {
      uVar3 = 0;
    }
    else {
      uVar3 = 1;
    }
    (**(code **)(*param_1 + 0x2a8))(param_1,param_3,iVar1);
  }
  return uVar3;
}

// ==== Java_com_adobe_ttpixel_extension_utils_ECUtils_bitmapFileWrite @ 00047380 ====


undefined4
Java_com_adobe_ttpixel_extension_utils_ECUtils_bitmapFileWrite
          (int *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,undefined4 param_5,
          undefined4 param_6,undefined4 param_7,undefined4 param_8,undefined4 param_9,
          undefined4 param_10,undefined4 param_11)

{
  int iVar1;
  int iVar2;
  undefined4 uVar3;
  
  iVar1 = (**(code **)(*param_1 + 0x2a4))(param_1,param_3,0);
  if (iVar1 == 0) {
    uVar3 = 0;
  }
  else {
    iVar2 = (**(code **)(*param_1 + 0x398))(param_1,param_4);
    if ((iVar2 == 0) ||
       (iVar2 = ECBitmapFileWrite(iVar1,iVar2,param_5,param_6,param_7,param_8,param_9,param_10,
                                  param_11), iVar2 == 0)) {
      uVar3 = 0;
    }
    else {
      uVar3 = 1;
    }
    (**(code **)(*param_1 + 0x2a8))(param_1,param_3,iVar1);
  }
  return uVar3;
}

// ==== Java_com_adobe_ttpixel_extension_utils_ECUtils_bitmapFileCreateFromBitmapData @ 00047440 ====


undefined4
Java_com_adobe_ttpixel_extension_utils_ECUtils_bitmapFileCreateFromBitmapData
          (int *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,undefined4 param_5,
          undefined4 param_6,undefined4 param_7)

{
  int iVar1;
  int iVar2;
  undefined4 uVar3;
  
  iVar1 = (**(code **)(*param_1 + 0x2a4))(param_1,param_3,0);
  if (iVar1 == 0) {
    uVar3 = 0;
  }
  else {
    iVar2 = (**(code **)(*param_1 + 0x398))(param_1,param_7);
    if ((iVar2 == 0) ||
       (iVar2 = ECBitmapFileCreateFromBitmapData(iVar1,param_4,param_5,param_6,iVar2), iVar2 == 0))
    {
      uVar3 = 0;
    }
    else {
      uVar3 = 1;
    }
    (**(code **)(*param_1 + 0x2a8))(param_1,param_3,iVar1);
  }
  return uVar3;
}

// ==== Java_com_adobe_ttpixel_extension_utils_ECUtils_bitmapFileCreateEmpty @ 000474e0 ====


undefined4
Java_com_adobe_ttpixel_extension_utils_ECUtils_bitmapFileCreateEmpty
          (int *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,undefined4 param_5,
          undefined4 param_6)

{
  int iVar1;
  undefined4 uVar2;
  
  iVar1 = (**(code **)(*param_1 + 0x2a4))(param_1,param_3,0);
  if (iVar1 == 0) {
    uVar2 = 0;
  }
  else {
    uVar2 = ECBitmapFileCreateEmpty(iVar1,param_4,param_5,param_6);
    (**(code **)(*param_1 + 0x2a8))(param_1,param_3,iVar1);
  }
  return uVar2;
}

// ==== Java_com_adobe_ttpixel_extension_utils_ECUtils_getScaledPixelsEx @ 00047544 ====


void Java_com_adobe_ttpixel_extension_utils_ECUtils_getScaledPixelsEx
               (int *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,
               undefined4 param_5,undefined4 param_6)

{
  undefined4 uVar1;
  undefined4 in_stack_00000014;
  undefined4 in_stack_00000018;
  undefined4 in_stack_00000028;
  
  uVar1 = (**(code **)(*param_1 + 0x398))
                    (param_1,param_3,in_stack_00000028,*param_1,in_stack_00000014);
  (**(code **)(*param_1 + 0x398))(param_1,in_stack_00000018);
  ECScaleBresenham(uVar1,param_4,param_5,param_6);
  return;
}

// ==== Java_com_adobe_ttpixel_extension_utils_ECUtils_bitmapDataFromFileEx @ 00047610 ====


undefined4
Java_com_adobe_ttpixel_extension_utils_ECUtils_bitmapDataFromFileEx
          (int *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,undefined4 param_5,
          undefined4 param_6,undefined4 param_7,undefined4 param_8,undefined4 param_9,
          undefined4 param_10,undefined4 param_11)

{
  int iVar1;
  undefined4 uVar2;
  
  iVar1 = (**(code **)(*param_1 + 0x2a4))(param_1,param_3,0);
  uVar2 = (**(code **)(*param_1 + 0x398))(param_1,param_4);
  uVar2 = ECBitmapDataFromFileEx
                    (iVar1,0,uVar2,param_5,param_6,param_7,param_8,param_9,param_10,param_11);
  if (iVar1 != 0) {
    (**(code **)(*param_1 + 0x2a8))(param_1,param_3,iVar1);
  }
  return uVar2;
}

// ==== Java_com_adobe_ttpixel_extension_utils_ECUtils_bitmapDataToFileEx @ 000476c4 ====


undefined4
Java_com_adobe_ttpixel_extension_utils_ECUtils_bitmapDataToFileEx
          (int *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,undefined4 param_5,
          undefined4 param_6,undefined4 param_7,undefined4 param_8,undefined4 param_9,
          undefined4 param_10,undefined4 param_11)

{
  int iVar1;
  undefined4 uVar2;
  
  iVar1 = (**(code **)(*param_1 + 0x2a4))(param_1,param_3,0);
  uVar2 = (**(code **)(*param_1 + 0x398))(param_1,param_4);
  uVar2 = ECBitmapDataToFileEx
                    (iVar1,0,uVar2,param_5,param_6,param_7,param_8,param_9,param_10,param_11);
  if (iVar1 != 0) {
    (**(code **)(*param_1 + 0x2a8))(param_1,param_3,iVar1);
  }
  return uVar2;
}

// ==== Java_com_adobe_ttpixel_extension_utils_ECUtils_uncompressBitmapDataEx @ 00047778 ====


void Java_com_adobe_ttpixel_extension_utils_ECUtils_uncompressBitmapDataEx
               (int *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,
               undefined4 param_5,undefined4 param_6)

{
  undefined4 uVar1;
  undefined4 uVar2;
  
  uVar1 = (**(code **)(*param_1 + 0x398))(param_1,param_5);
  uVar2 = (**(code **)(*param_1 + 0x398))(param_1,param_3);
  ECUnCompressBitmapDataEx(uVar2,param_4,uVar1,param_6);
  return;
}

// ==== Java_com_adobe_ttpixel_extension_utils_ECUtils_moveBitmapDataEx @ 000477d0 ====


undefined4
Java_com_adobe_ttpixel_extension_utils_ECUtils_moveBitmapDataEx
          (int *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,undefined4 param_5,
          undefined4 param_6,undefined4 param_7,undefined4 param_8,undefined4 param_9,
          undefined4 param_10)

{
  undefined4 uVar1;
  
  uVar1 = (**(code **)(*param_1 + 0x398))(param_1,param_3);
  ECMoveBitmapDataEx(uVar1,param_4,param_5,param_6,param_7,param_8,param_9,param_10);
  return 1;
}

// ==== Java_com_adobe_ttpixel_extension_utils_ECUtils_getPixelsBitmapEx @ 00047828 ====


void Java_com_adobe_ttpixel_extension_utils_ECUtils_getPixelsBitmapEx
               (int *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,
               undefined4 param_5,undefined4 param_6,undefined4 param_7,undefined4 param_8,
               undefined4 param_9,undefined4 param_10)

{
  undefined4 uVar1;
  undefined4 in_stack_00000024;
  
  uVar1 = (**(code **)(*param_1 + 0x398))(param_1,param_3,in_stack_00000024,*param_1,param_10);
  (**(code **)(*param_1 + 0x398))(param_1,param_7);
  ECGetPixelsBitmapEx(uVar1,param_4,param_5,param_6);
  return;
}

// ==== Java_com_adobe_ttpixel_extension_utils_ECUtils_lz4Free @ 00047900 ====


void Java_com_adobe_ttpixel_extension_utils_ECUtils_lz4Free
               (int *param_1,undefined4 param_2,int param_3)

{
  void *__ptr;
  
  if (param_3 == 0) {
    return;
  }
  __ptr = (void *)(**(code **)(*param_1 + 0x398))();
  if (__ptr == (void *)0x0) {
    return;
  }
  free(__ptr);
  return;
}

// ==== Java_com_adobe_ttpixel_extension_utils_ECUtils_lz4Uncompress @ 00047928 ====


int Java_com_adobe_ttpixel_extension_utils_ECUtils_lz4Uncompress
              (int *param_1,undefined4 param_2,int param_3,int param_4)

{
  int iVar1;
  undefined4 local_18;
  undefined4 local_14;
  
  if ((param_3 != 0) && (iVar1 = (**(code **)(*param_1 + 0x398))(), iVar1 != 0)) {
    if (param_4 == 0) {
      local_14 = 0;
    }
    else {
      local_14 = (**(code **)(*param_1 + 0x398))(param_1,param_4);
    }
    iVar1 = ECLz4Uncompress(iVar1,&local_14,&local_18);
    if (iVar1 != 0) {
      if (param_4 != 0) {
        return param_4;
      }
      iVar1 = (**(code **)(*param_1 + 0x394))(param_1,local_14,local_18,0);
      return iVar1;
    }
  }
  return 0;
}

// ==== Java_com_adobe_ttpixel_extension_utils_ECUtils_lz4GetMaxCompressDestLength @ 000479cc ====


void Java_com_adobe_ttpixel_extension_utils_ECUtils_lz4GetMaxCompressDestLength
               (undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  ECLz4GetMaxCompressDestLength(param_3);
  return;
}

// ==== Java_com_adobe_ttpixel_extension_utils_ECUtils_lz4Compress @ 000479d4 ====


/* WARNING: Removing unreachable block (ram,0x00047b04) */

undefined4
Java_com_adobe_ttpixel_extension_utils_ECUtils_lz4Compress
          (int *param_1,undefined4 param_2,int param_3,int param_4,char param_5)

{
  int iVar1;
  int iVar2;
  int *piVar3;
  int iVar4;
  undefined4 uVar5;
  code *pcVar6;
  longlong lVar7;
  undefined4 local_28;
  undefined4 local_24 [2];
  
  if (param_3 != 0) {
    iVar2 = (**(code **)(*param_1 + 0x398))(param_1,param_3);
    lVar7 = (**(code **)(*param_1 + 0x39c))(param_1,param_3);
    if (iVar2 != 0 && 0 < lVar7) {
      if (param_4 == 0) {
        local_24[0] = 0;
      }
      else {
        local_24[0] = (**(code **)(*param_1 + 0x398))(param_1,param_4);
      }
      iVar2 = ECLz4Compress(iVar2,(int)lVar7,local_24,&local_28,param_5 == '\x01');
      if (iVar2 != 0) {
        piVar3 = (int *)(DAT_00047b8c + 0x47aac);
        iVar2 = *piVar3;
        if (iVar2 == 0) {
          iVar2 = (**(code **)(*param_1 + 0x18))(param_1,*(undefined4 *)(DAT_00047b98 + 0x47b24));
          *piVar3 = iVar2;
          if (iVar2 == 0) {
            return 0;
          }
        }
        iVar1 = DAT_00047b90;
        iVar4 = *(int *)(DAT_00047b90 + 0x47ac4);
        if (iVar4 == 0) {
          iVar4 = (**(code **)(*param_1 + 0x84))
                            (param_1,iVar2,*(undefined4 *)(DAT_00047b9c + 0x47b54),
                             *(undefined4 *)(DAT_00047b9c + 0x47b58));
          *(int *)(iVar1 + 0x47ac4) = iVar4;
          if (iVar4 == 0) {
            return 0;
          }
        }
        uVar5 = *(undefined4 *)(DAT_00047b94 + 0x47ae0);
        pcVar6 = *(code **)(*param_1 + 0x70);
        if (param_4 == 0) {
          param_4 = (**(code **)(*param_1 + 0x394))(param_1,local_24[0],local_28,0);
        }
        uVar5 = (*pcVar6)(param_1,uVar5,iVar4,local_28,param_4);
        return uVar5;
      }
    }
  }
  return 0;
}

// ==== Java_com_adobe_ttpixel_extension_utils_ECUtils_setPixelsEx @ 00047ba0 ====


void Java_com_adobe_ttpixel_extension_utils_ECUtils_setPixelsEx
               (int *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,int param_5,
               undefined4 param_6,undefined4 param_7)

{
  undefined4 uVar1;
  int iVar2;
  undefined4 in_stack_00000018;
  undefined4 in_stack_0000001c;
  
  uVar1 = (**(code **)(*param_1 + 0x398))
                    (param_1,param_3,in_stack_0000001c,*param_1,in_stack_00000018);
  iVar2 = (**(code **)(*param_1 + 0x398))(param_1,param_4);
  ECSetPixelsEx(uVar1,iVar2 + param_5,param_6,param_7);
  return;
}

// ==== Java_com_adobe_ttpixel_extension_utils_ECUtils_getPixelsEx @ 00047c54 ====


void Java_com_adobe_ttpixel_extension_utils_ECUtils_getPixelsEx
               (int *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,
               undefined4 param_5,undefined4 param_6)

{
  undefined4 uVar1;
  undefined4 uVar2;
  
  uVar1 = (**(code **)(*param_1 + 0x398))(param_1,param_3);
  uVar2 = (**(code **)(*param_1 + 0x398))(param_1,param_4);
  ECGetPixelsEx(uVar1,uVar2,param_5,param_6);
  return;
}

// ==== Java_com_adobe_ttpixel_extension_utils_FnCompressBitmapRLE_nativeCompressBitmapRLE @ 00047d04 ====


void Java_com_adobe_ttpixel_extension_utils_FnCompressBitmapRLE_nativeCompressBitmapRLE
               (int *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,
               undefined4 param_5,undefined4 param_6)

{
  undefined4 uVar1;
  undefined4 uVar2;
  
  uVar1 = (**(code **)(*param_1 + 0x398))(param_1,param_3);
  uVar2 = (**(code **)(*param_1 + 0x398))(param_1,param_6);
  ECCompressBitmapRLE(uVar1,param_4,param_5,uVar2);
  return;
}

// ==== Java_com_adobe_ttpixel_extension_utils_ECUtils_isolateColor @ 00047d60 ====


undefined4
Java_com_adobe_ttpixel_extension_utils_ECUtils_isolateColor
          (int *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,int param_5,
          int param_6)

{
  undefined4 uVar1;
  undefined4 uVar2;
  
  if (param_6 == 0 || param_5 == 0) {
    return 0xfffffc18;
  }
  uVar1 = (**(code **)(*param_1 + 0x398))();
  uVar2 = (**(code **)(*param_1 + 0x398))(param_1,param_6);
  uVar1 = ECIsolateColor(param_3,param_4,uVar1,uVar2);
  return uVar1;
}

// ==== Java_com_adobe_ttpixel_extension_bigdata_ECBitmapPreflight_dispose @ 00047df4 ====


void Java_com_adobe_ttpixel_extension_bigdata_ECBitmapPreflight_dispose
               (undefined4 param_1,undefined4 param_2,void *param_3)

{
  if (param_3 == (void *)0x0) {
    return;
  }
  free(param_3);
  return;
}

// ==== Java_com_adobe_ttpixel_extension_bigdata_ECBitmapPreflight_outputToString @ 00047e00 ====


undefined4
Java_com_adobe_ttpixel_extension_bigdata_ECBitmapPreflight_outputToString
          (int *param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  undefined4 uVar2;
  void *local_c;
  
  if ((param_3 == 0) || (iVar1 = ECBitmapPreflightOutputToString(param_3,&local_c), iVar1 == 0)) {
    uVar2 = 0;
  }
  else {
    uVar2 = (**(code **)(*param_1 + 0x29c))(param_1,local_c);
    free(local_c);
  }
  return uVar2;
}

// ==== Java_com_adobe_ttpixel_extension_bigdata_ECBitmapPreflight_perform @ 00047e5c ====


undefined4
Java_com_adobe_ttpixel_extension_bigdata_ECBitmapPreflight_perform
          (int *param_1,undefined4 param_2,undefined4 *param_3,undefined4 param_4,undefined4 param_5
          ,undefined4 param_6,undefined4 param_7,undefined4 param_8,undefined4 param_9,
          undefined4 param_10)

{
  undefined4 uVar1;
  
  if (param_3 != (undefined4 *)0x0) {
    uVar1 = (**(code **)(*param_1 + 0x398))(param_1,param_10);
    param_3[3] = param_8;
    *param_3 = param_5;
    param_3[4] = param_9;
    param_3[1] = param_6;
    param_3[2] = param_7;
    param_3[5] = uVar1;
    uVar1 = ECBitmapPreflightPerform(param_3);
    return uVar1;
  }
  return 0;
}

// ==== Java_com_adobe_ttpixel_extension_bigdata_ECBitmapPreflight_create @ 00047eb8 ====


undefined8 Java_com_adobe_ttpixel_extension_bigdata_ECBitmapPreflight_create(void)

{
  int iVar1;
  int aiStack_c [3];
  
  iVar1 = ECBitmapPreflightCreate(aiStack_c);
  if (iVar1 == 0) {
    aiStack_c[0] = 0;
    iVar1 = 0;
  }
  else {
    iVar1 = aiStack_c[0] >> 0x1f;
  }
  return CONCAT44(iVar1,aiStack_c[0]);
}

// ==== Java_com_adobe_ttpixel_extension_TTPixelExtensionContextPressureJaJa_native_1destroyButtonEventsQueue @ 00047eec ====


void Java_com_adobe_ttpixel_extension_TTPixelExtensionContextPressureJaJa_native_1destroyButtonEventsQueue
               (undefined4 param_1,undefined4 param_2,int *param_3)

{
  if (param_3 == (int *)0x0) {
    return;
  }
  (**(code **)(*param_3 + 4))(param_3);
  return;
}

// ==== Java_com_adobe_ttpixel_extension_TTPixelExtensionContextPressureJaJa_native_1destroyPressureEventsQueue @ 00047f0c ====


void Java_com_adobe_ttpixel_extension_TTPixelExtensionContextPressureJaJa_native_1destroyPressureEventsQueue
               (undefined4 param_1,undefined4 param_2,int *param_3)

{
  if (param_3 == (int *)0x0) {
    return;
  }
  (**(code **)(*param_3 + 4))(param_3);
  return;
}

// ==== Java_com_adobe_ttpixel_extension_TTPixelExtensionContextPressureJaJa_native_1setButtonState @ 00047f2c ====


void Java_com_adobe_ttpixel_extension_TTPixelExtensionContextPressureJaJa_native_1setButtonState
               (undefined4 param_1,undefined4 param_2,int param_3,undefined4 param_4,
               undefined1 param_5)

{
  uint uVar1;
  int extraout_r1;
  int iVar2;
  undefined4 *puVar3;
  undefined8 uVar4;
  undefined4 local_28;
  undefined4 uStack_24;
  
  uVar4 = ECUtilsTimerGetTimestamp();
  if (*(char *)(param_3 + 0xc) == '\0') {
    iVar2 = *(int *)(param_3 + 0x14);
    uVar1 = *(uint *)(param_3 + 4);
    if (uVar1 <= (uint)((iVar2 - *(int *)(param_3 + 0x18)) - *(int *)(param_3 + 0x10))) {
      return;
    }
  }
  else {
    iVar2 = *(int *)(param_3 + 0x14);
    uVar1 = *(uint *)(param_3 + 4);
  }
  __aeabi_uidivmod(iVar2,uVar1);
  local_28 = CONCAT31(local_28._1_3_,param_5);
  puVar3 = (undefined4 *)(*(int *)(param_3 + 8) + extraout_r1 * 0x10);
  *puVar3 = local_28;
  puVar3[1] = uStack_24;
  *(undefined8 *)(puVar3 + 2) = uVar4;
  *(int *)(param_3 + 0x14) = iVar2 + 1;
  if (*(uint *)(param_3 + 4) <
      (uint)(((iVar2 + 1) - *(int *)(param_3 + 0x18)) - *(int *)(param_3 + 0x10))) {
    *(int *)(param_3 + 0x18) = *(int *)(param_3 + 0x18) + 1;
  }
  return;
}

// ==== Java_com_adobe_ttpixel_extension_TTPixelExtensionContextPressureJaJa_native_1setPressure @ 00047fd4 ====


void Java_com_adobe_ttpixel_extension_TTPixelExtensionContextPressureJaJa_native_1setPressure
               (undefined4 param_1,undefined4 param_2,int param_3,undefined4 param_4,
               undefined4 param_5)

{
  int extraout_r1;
  int iVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  undefined8 uVar6;
  
  uVar6 = ECUtilsTimerGetTimestamp();
  if (*(char *)(param_3 + 0xc) == '\0') {
    iVar3 = *(int *)(param_3 + 0x14);
    iVar4 = *(int *)(param_3 + 0x18);
    iVar5 = *(int *)(param_3 + 0x10);
    uVar2 = *(uint *)(param_3 + 4);
    if (uVar2 <= (uint)((iVar3 - iVar4) - iVar5)) {
      return;
    }
  }
  else {
    iVar3 = *(int *)(param_3 + 0x14);
    uVar2 = *(uint *)(param_3 + 4);
    iVar4 = *(int *)(param_3 + 0x18);
    iVar5 = *(int *)(param_3 + 0x10);
  }
  __aeabi_uidivmod(iVar3,uVar2);
  iVar1 = *(int *)(param_3 + 8);
  *(int *)(param_3 + 0x14) = iVar3 + 1;
  *(undefined4 *)(iVar1 + extraout_r1 * 0x10) = param_5;
  *(undefined8 *)(iVar1 + extraout_r1 * 0x10 + 8) = uVar6;
  if (uVar2 < (uint)(((iVar3 + 1) - iVar4) - iVar5)) {
    *(int *)(param_3 + 0x18) = iVar4 + 1;
  }
  return;
}

// ==== Java_com_adobe_ttpixel_extension_TTPixelExtensionContextPressureJaJa_native_1createPressureEventsQueue @ 0004816c ====


longlong Java_com_adobe_ttpixel_extension_TTPixelExtensionContextPressureJaJa_native_1createPressureEventsQueue
                   (void)

{
  undefined4 uVar1;
  int *piVar2;
  undefined4 *puVar3;
  int iVar4;
  undefined4 *puVar5;
  int iVar6;
  
  piVar2 = (int *)operator_new(0x44);
  iVar6 = DAT_00048258 + 0x4818c;
  iVar4 = *(int *)(iVar6 + DAT_0004825c);
  piVar2[1] = 0x100;
  piVar2[6] = 0;
  piVar2[2] = 0;
  *(undefined1 *)(piVar2 + 3) = 1;
  *piVar2 = iVar4 + 8;
  piVar2[4] = 0;
  piVar2[5] = 0;
  puVar3 = (undefined4 *)operator_new__(0x1000);
  uVar1 = DAT_00048254;
  puVar5 = puVar3;
  do {
    *puVar5 = uVar1;
    puVar5[2] = 0;
    puVar5[3] = 0;
    iVar4 = DAT_00048260;
    puVar5 = puVar5 + 4;
  } while (puVar5 != puVar3 + 0x400);
  piVar2[2] = (int)puVar3;
  iVar4 = *(int *)(iVar6 + iVar4);
  *(undefined1 *)(piVar2 + 0x10) = 1;
  piVar2[0xf] = 0;
  *piVar2 = iVar4 + 8;
  piVar2[7] = 0;
  piVar2[8] = 0;
  piVar2[9] = 0;
  piVar2[10] = 0;
  piVar2[0xb] = 0;
  piVar2[0xc] = 0;
  piVar2[0xd] = 0;
  piVar2[0xe] = 0;
  return (longlong)(int)piVar2;
}

// ==== Java_com_adobe_ttpixel_extension_TTPixelExtensionContextPressureJaJa_native_1createButtonEventsQueue @ 00048264 ====


longlong Java_com_adobe_ttpixel_extension_TTPixelExtensionContextPressureJaJa_native_1createButtonEventsQueue
                   (void)

{
  int *piVar1;
  undefined1 *puVar2;
  int iVar3;
  undefined1 *puVar4;
  int iVar5;
  
  piVar1 = (int *)operator_new(0x44);
  iVar5 = DAT_00048348 + 0x48284;
  iVar3 = *(int *)(iVar5 + DAT_0004834c);
  piVar1[1] = 0x10;
  piVar1[6] = 0;
  piVar1[2] = 0;
  *(undefined1 *)(piVar1 + 3) = 1;
  *piVar1 = iVar3 + 8;
  piVar1[4] = 0;
  piVar1[5] = 0;
  puVar2 = (undefined1 *)operator_new__(0x100);
  puVar4 = puVar2;
  do {
    *puVar4 = 0;
    *(undefined4 *)(puVar4 + 8) = 0;
    *(undefined4 *)(puVar4 + 0xc) = 0;
    iVar3 = DAT_00048350;
    puVar4 = puVar4 + 0x10;
  } while (puVar4 != puVar2 + 0x100);
  piVar1[2] = (int)puVar2;
  piVar1[0xf] = 0;
  iVar3 = *(int *)(iVar5 + iVar3);
  *(undefined1 *)(piVar1 + 0x10) = 0;
  piVar1[7] = 0;
  *piVar1 = iVar3 + 8;
  piVar1[8] = 0;
  piVar1[9] = 0;
  piVar1[10] = 0;
  piVar1[0xb] = 0;
  piVar1[0xc] = 0;
  piVar1[0xd] = 0;
  piVar1[0xe] = 0;
  return (longlong)(int)piVar1;
}

// ==== Java_com_adobe_ttpixel_extension_TTPixelExtensionContextPressureJaJa_native_1getPressure @ 00048474 ====


float Java_com_adobe_ttpixel_extension_TTPixelExtensionContextPressureJaJa_native_1getPressure
                (undefined4 param_1,undefined4 param_2,int param_3,undefined4 param_4,
                undefined4 param_5,undefined4 param_6)

{
  int iVar1;
  float fVar2;
  float fVar3;
  uint uVar4;
  int extraout_r1;
  int iVar5;
  int extraout_r1_00;
  int extraout_r1_01;
  int iVar6;
  int extraout_r1_02;
  int extraout_r1_03;
  int extraout_r1_04;
  int iVar7;
  int iVar8;
  uint uVar9;
  uint uVar10;
  float *pfVar11;
  float fVar12;
  uint uVar13;
  int iVar14;
  uint uVar15;
  float fVar16;
  float fVar17;
  undefined4 uVar18;
  float fVar19;
  int iVar20;
  float fVar21;
  float fVar22;
  undefined8 uVar23;
  
  iVar1 = *(int *)(param_3 + 0x18);
  iVar8 = *(int *)(param_3 + 0x10);
  iVar7 = *(int *)(param_3 + 0x14);
  iVar14 = iVar7 - iVar1;
  *(int *)(param_3 + 0x1c) = *(int *)(param_3 + 0x1c) + 1;
  if (iVar14 == iVar8) {
    fVar12 = 0.0;
    *(int *)(param_3 + 0x28) = *(int *)(param_3 + 0x28) + 1;
  }
  else {
    uVar23 = __fixunsdfdi(param_5,param_6);
    uVar4 = (uint)((ulonglong)uVar23 >> 0x20);
    fVar2 = (float)uVar23;
    __aeabi_uidivmod(iVar8 + iVar1,*(undefined4 *)(param_3 + 4));
    iVar20 = *(int *)(param_3 + 8);
    iVar5 = iVar20 + extraout_r1 * 0x10;
    uVar9 = *(uint *)(iVar5 + 0xc);
    if ((uVar4 <= uVar9 && uVar9 != uVar4) ||
       ((uVar9 == uVar4 && ((uint)fVar2 < (uint)*(float *)(iVar5 + 8))))) {
      fVar12 = 0.0;
      *(int *)(param_3 + 0x2c) = *(int *)(param_3 + 0x2c) + 1;
    }
    else {
      uVar13 = 0;
      iVar5 = 1;
      uVar15 = iVar14 - iVar8;
      uVar9 = uVar15 - 1;
      uVar18 = *(undefined4 *)(param_3 + 4);
      iVar14 = *(int *)(param_3 + 0x20) + 1;
      *(int *)(param_3 + 0x20) = iVar14;
      if (uVar9 != 0) {
        do {
          uVar15 = uVar15 >> 1;
          __aeabi_uidivmod(iVar8 + iVar1,uVar18);
          __aeabi_uidivmod(uVar15 + extraout_r1_00,uVar18);
          iVar6 = iVar20 + extraout_r1_01 * 0x10;
          uVar10 = *(uint *)(iVar6 + 0xc);
          if ((uVar4 <= uVar10 && uVar10 != uVar4) ||
             ((uVar10 == uVar4 && ((uint)fVar2 < (uint)*(float *)(iVar6 + 8))))) {
            uVar9 = uVar15 - 1;
            *(int *)(param_3 + 0x20) = iVar14 + 1;
          }
          else {
            *(int *)(param_3 + 0x20) = iVar14 + 1;
            uVar13 = uVar15;
          }
          if (iVar5 == 0x1e) {
            *(int *)(param_3 + 0x30) = *(int *)(param_3 + 0x30) + 1;
            return 0.0;
          }
          iVar14 = iVar14 + 1;
          uVar15 = uVar13 + uVar9 + 1;
          iVar5 = iVar5 + 1;
        } while (uVar13 != uVar9);
      }
      __aeabi_uidivmod(iVar8 + iVar1,uVar18);
      __aeabi_uidivmod(extraout_r1_02 + uVar13,uVar18);
      iVar14 = iVar20 + extraout_r1_03 * 0x10;
      fVar12 = *(float *)(iVar20 + extraout_r1_03 * 0x10);
      fVar16 = *(float *)(iVar14 + 8);
      fVar17 = *(float *)(iVar14 + 0xc);
      if (uVar13 < ((iVar7 - iVar1) - iVar8) - 1U) {
        __aeabi_uidivmod(extraout_r1_02 + 1 + uVar13,uVar18);
        pfVar11 = (float *)(iVar20 + extraout_r1_04 * 0x10);
        fVar22 = *pfVar11;
        fVar19 = pfVar11[2];
        fVar21 = pfVar11[3];
        if (((fVar16 == fVar19) && (fVar17 == fVar21)) || (*(char *)(param_3 + 0x40) == '\0')) {
          *(int *)(param_3 + 0x38) = *(int *)(param_3 + 0x38) + 1;
        }
        else {
          fVar3 = (float)__floatundisf((int)fVar2 - (int)fVar16,
                                       uVar4 - ((int)fVar17 + (uint)((uint)fVar2 < (uint)fVar16)));
          fVar17 = (float)__floatundisf((int)fVar19 - (int)fVar16,
                                        (int)fVar21 -
                                        ((int)fVar17 + (uint)((uint)fVar19 < (uint)fVar16)));
          *(int *)(param_3 + 0x3c) = *(int *)(param_3 + 0x3c) + 1;
          fVar12 = fVar12 + (fVar3 * (fVar22 - fVar12)) / fVar17;
        }
      }
      else {
        *(int *)(param_3 + 0x34) = *(int *)(param_3 + 0x34) + 1;
      }
      if (*(uint *)(param_3 + 0x24) < (uint)((int)fVar2 - (int)fVar16)) {
        *(int *)(param_3 + 0x24) = (int)fVar2 - (int)fVar16;
      }
    }
  }
  return fVar12;
}

// ==== Java_com_adobe_ttpixel_extension_TTPixelExtensionContextPressureJaJa_native_1getButtonState @ 00048728 ====


uint Java_com_adobe_ttpixel_extension_TTPixelExtensionContextPressureJaJa_native_1getButtonState
               (undefined4 param_1,undefined4 param_2,int param_3,undefined4 param_4,
               undefined4 param_5,undefined4 param_6)

{
  uint uVar1;
  int extraout_r1;
  int iVar2;
  int extraout_r1_00;
  int extraout_r1_01;
  int iVar3;
  int extraout_r1_02;
  int extraout_r1_03;
  uint *puVar4;
  int extraout_r1_04;
  int iVar5;
  int iVar6;
  uint uVar7;
  uint uVar8;
  uint uVar9;
  undefined4 uVar10;
  int iVar11;
  uint uVar12;
  uint uVar13;
  int iVar14;
  undefined8 uVar15;
  
  iVar5 = *(int *)(param_3 + 0x18);
  iVar6 = *(int *)(param_3 + 0x10);
  iVar11 = *(int *)(param_3 + 0x14) - iVar5;
  *(int *)(param_3 + 0x1c) = *(int *)(param_3 + 0x1c) + 1;
  if (iVar11 == iVar6) {
    uVar13 = 0;
    *(int *)(param_3 + 0x28) = *(int *)(param_3 + 0x28) + 1;
  }
  else {
    uVar15 = __fixunsdfdi(param_5,param_6);
    uVar13 = (uint)((ulonglong)uVar15 >> 0x20);
    uVar1 = (uint)uVar15;
    __aeabi_uidivmod(iVar6 + iVar5,*(undefined4 *)(param_3 + 4));
    iVar14 = *(int *)(param_3 + 8);
    iVar2 = iVar14 + extraout_r1 * 0x10;
    uVar7 = *(uint *)(iVar2 + 0xc);
    if ((uVar13 <= uVar7 && uVar7 != uVar13) ||
       ((uVar7 == uVar13 && (uVar1 < *(uint *)(iVar2 + 8))))) {
      uVar13 = 0;
      *(int *)(param_3 + 0x2c) = *(int *)(param_3 + 0x2c) + 1;
    }
    else {
      uVar9 = 0;
      iVar2 = 1;
      uVar7 = iVar11 - iVar6;
      uVar12 = uVar7 - 1;
      uVar10 = *(undefined4 *)(param_3 + 4);
      iVar11 = *(int *)(param_3 + 0x20) + 1;
      *(int *)(param_3 + 0x20) = iVar11;
      if (uVar12 != 0) {
        do {
          uVar7 = uVar7 >> 1;
          __aeabi_uidivmod(iVar6 + iVar5,uVar10);
          __aeabi_uidivmod(uVar7 + extraout_r1_00,uVar10);
          iVar3 = iVar14 + extraout_r1_01 * 0x10;
          uVar8 = *(uint *)(iVar3 + 0xc);
          if ((uVar13 <= uVar8 && uVar8 != uVar13) ||
             ((uVar8 == uVar13 && (uVar1 < *(uint *)(iVar3 + 8))))) {
            uVar12 = uVar7 - 1;
            *(int *)(param_3 + 0x20) = iVar11 + 1;
          }
          else {
            *(int *)(param_3 + 0x20) = iVar11 + 1;
            uVar9 = uVar7;
          }
          if (iVar2 == 0x1e) {
            *(int *)(param_3 + 0x30) = *(int *)(param_3 + 0x30) + 1;
            return 0;
          }
          iVar11 = iVar11 + 1;
          uVar7 = uVar12 + uVar9 + 1;
          iVar2 = iVar2 + 1;
        } while (uVar9 != uVar12);
      }
      __aeabi_uidivmod(iVar6 + iVar5,uVar10);
      __aeabi_uidivmod(uVar9 + extraout_r1_02,uVar10);
      puVar4 = (uint *)(iVar14 + extraout_r1_03 * 0x10);
      uVar7 = puVar4[2];
      uVar12 = puVar4[3];
      uVar13 = *puVar4 & 0xff;
      if (uVar9 < ((*(int *)(param_3 + 0x14) - iVar5) - iVar6) - 1U) {
        __aeabi_uidivmod(extraout_r1_02 + 1 + uVar9,uVar10);
        puVar4 = (uint *)(iVar14 + extraout_r1_04 * 0x10);
        if (((uVar7 == puVar4[2]) && (uVar12 == puVar4[3])) || (*(char *)(param_3 + 0x40) == '\0'))
        {
          *(int *)(param_3 + 0x38) = *(int *)(param_3 + 0x38) + 1;
        }
        else {
          iVar5 = __aeabi_idiv(((*puVar4 & 0xff) - uVar13) * (uVar1 - (uVar7 & 0xff) & 0xff),
                               puVar4[2] - (uVar7 & 0xff) & 0xff);
          *(int *)(param_3 + 0x3c) = *(int *)(param_3 + 0x3c) + 1;
          uVar13 = uVar13 + iVar5 & 0xff;
        }
      }
      else {
        *(int *)(param_3 + 0x34) = *(int *)(param_3 + 0x34) + 1;
      }
      if (*(uint *)(param_3 + 0x24) < uVar1 - uVar7) {
        *(uint *)(param_3 + 0x24) = uVar1 - uVar7;
      }
    }
  }
  return uVar13;
}

// ==== Java_com_adobe_ttpixel_extension_utils_AIRRuntimeHelper_updateCompatibleBitmap @ 000489e4 ====


undefined4
Java_com_adobe_ttpixel_extension_utils_AIRRuntimeHelper_updateCompatibleBitmap
          (undefined4 param_1,undefined4 param_2,int param_3,int param_4)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  undefined4 uVar6;
  int local_64;
  int local_60;
  int local_5c;
  int local_58;
  void *local_54;
  int local_38;
  int local_34;
  undefined4 local_30;
  int local_2c;
  void *local_24;
  
  if (param_4 == 0 || param_3 == 0) {
    uVar6 = 0;
  }
  else {
    iVar3 = ANativeWindow_fromSurface(param_1,param_3);
    if (iVar3 == 0) {
      __android_log_print(3,DAT_00048bd4 + 0x48b58,DAT_00048bd8 + 0x48b5c,DAT_00048bd4 + 0x48b58);
      uVar6 = 0;
    }
    else {
      iVar4 = ANativeWindow_lock(iVar3,&local_64,0);
      iVar2 = local_5c;
      iVar1 = local_60;
      if (iVar4 < 0) {
        uVar6 = 0;
        __android_log_print(3,DAT_00048bdc + 0x48b7c,DAT_00048be0 + 0x48b80,DAT_00048bdc + 0x48b7c);
      }
      else {
        if (local_58 == 4) {
          iVar4 = 2;
        }
        else {
          iVar4 = 4;
        }
        iVar5 = AndroidBitmap_getInfo(param_1,param_4,&local_38);
        if (iVar5 < 0) {
          uVar6 = 0;
          __android_log_print(3,DAT_00048be4 + 0x48ba0,DAT_00048be8 + 0x48ba4,DAT_00048be4 + 0x48ba0
                             );
        }
        else if ((((local_64 == local_38) && (local_60 == local_34)) &&
                 (iVar5 = __aeabi_uidiv(local_30,iVar4), local_5c == iVar5)) &&
                ((local_58 == local_2c || ((local_2c == 1 && (local_58 == 2)))))) {
          local_24 = (void *)0x0;
          AndroidBitmap_lockPixels(param_1,param_4);
          if (local_24 == (void *)0x0) {
            __android_log_print(3,DAT_00048bec + 0x48bc0,DAT_00048bf0 + 0x48bc4,
                                DAT_00048bec + 0x48bc0);
            uVar6 = 0;
          }
          else {
            uVar6 = 1;
            memcpy(local_24,local_54,iVar4 * iVar2 * iVar1);
            AndroidBitmap_unlockPixels(param_1,param_4);
          }
        }
        else {
          uVar6 = 0;
          __android_log_print(3,DAT_00048bcc + 0x48a90,DAT_00048bd0 + 0x48a94,DAT_00048bcc + 0x48a90
                             );
        }
        ANativeWindow_unlockAndPost(iVar3);
      }
      ANativeWindow_release(iVar3);
    }
  }
  return uVar6;
}

// ==== Java_com_adobe_ttpixel_extension_utils_AIRRuntimeHelper_createCompatibleBitmap @ 00048c28 ====


undefined4
Java_com_adobe_ttpixel_extension_utils_AIRRuntimeHelper_createCompatibleBitmap
          (_JNIEnv *param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  undefined4 uVar5;
  undefined4 local_44;
  int local_40;
  int local_3c;
  int local_38;
  
  iVar4 = DAT_00048d64 + 0x48c44;
  if ((param_3 == 0) || (iVar1 = ANativeWindow_fromSurface(), iVar1 == 0)) {
    uVar5 = 0;
  }
  else {
    iVar2 = ANativeWindow_lock(iVar1,&local_44,0);
    if (iVar2 < 0) {
      uVar5 = 0;
    }
    else {
      iVar2 = (**(code **)(*(int *)param_1 + 0x2cc))(param_1,local_3c * local_40);
      if (iVar2 == 0) {
        uVar5 = 0;
      }
      else {
        if (local_38 == 4) {
          uVar5 = **(undefined4 **)(iVar4 + DAT_00048d78);
        }
        else {
          uVar5 = **(undefined4 **)(iVar4 + DAT_00048d6c);
        }
        iVar3 = (**(code **)(*(int *)param_1 + 0x244))
                          (param_1,**(undefined4 **)(iVar4 + DAT_00048d68),uVar5);
        if (iVar3 == 0) {
          uVar5 = 0;
        }
        else {
          uVar5 = _JNIEnv::CallStaticObjectMethod
                            (param_1,(_jclass *)**(undefined4 **)(iVar4 + DAT_00048d70),
                             (_jmethodID *)**(undefined4 **)(iVar4 + DAT_00048d74),iVar2,0,local_3c,
                             local_44,local_40,iVar3);
        }
        (**(code **)(*(int *)param_1 + 0x5c))(param_1,iVar2);
      }
      ANativeWindow_unlockAndPost(iVar1);
    }
    ANativeWindow_release(iVar1);
  }
  return uVar5;
}

// ==== Java_com_adobe_ttpixel_extension_TTPixelExtensionContextQuickSelection_createQuickSelectTool @ 00048e10 ====


int Java_com_adobe_ttpixel_extension_TTPixelExtensionContextQuickSelection_createQuickSelectTool
              (int *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,int param_5,
              int param_6)

{
  uchar *puVar1;
  int iVar2;
  QuickSelectTool *pQVar3;
  int iVar4;
  uchar *puVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  
  iVar2 = DAT_00048f54;
  iVar7 = DAT_00048f50 + 0x48e28;
  pQVar3 = (QuickSelectTool *)
           (**(code **)(*param_1 + 0x194))(param_1,param_3,**(undefined4 **)(iVar7 + DAT_00048f54));
  iVar4 = (**(code **)(*param_1 + 0x398))(param_1,param_4);
  if (iVar4 != 0) {
    if (pQVar3 != (QuickSelectTool *)0x0) {
      QuickSelectTool::~QuickSelectTool(pQVar3);
      operator_delete(pQVar3);
    }
    iVar6 = DAT_00048f58;
    iVar8 = param_5 * param_6;
    *(int *)(DAT_00048f58 + 0x48ea4) = param_5;
    *(int *)(iVar6 + 0x48ea8) = param_6;
    puVar5 = (uchar *)operator_new__(param_6 * param_5 * 3);
    if (iVar8 != 0) {
      iVar6 = 0;
      do {
        puVar5[iVar6] = *(uchar *)(iVar4 + 1);
        puVar5[iVar6 + iVar8] = *(uchar *)(iVar4 + 2);
        puVar1 = (uchar *)(iVar4 + 3);
        iVar4 = iVar4 + 4;
        puVar5[iVar6 + iVar8 * 2] = *puVar1;
        iVar6 = iVar6 + 1;
      } while (iVar8 - iVar6 != 0);
    }
    pQVar3 = (QuickSelectTool *)operator_new(0xf8);
    QuickSelectTool::QuickSelectTool(pQVar3,puVar5,param_6,param_5);
    (**(code **)(*param_1 + 0x1b8))
              (param_1,param_3,**(undefined4 **)(iVar7 + iVar2),*param_1,pQVar3,(int)pQVar3 >> 0x1f)
    ;
    iVar4 = 1;
  }
  return iVar4;
}

// ==== Java_com_adobe_ttpixel_extension_TTPixelExtensionContextQuickSelection_switchMode @ 00048f5c ====


undefined4
Java_com_adobe_ttpixel_extension_TTPixelExtensionContextQuickSelection_switchMode
          (int *param_1,undefined4 param_2,undefined4 param_3,int param_4)

{
  QuickSelectTool *this;
  
  this = (QuickSelectTool *)
         (**(code **)(*param_1 + 0x194))(param_1,param_3,**(undefined4 **)(DAT_00048f9c + 0x48f78));
  if (this != (QuickSelectTool *)0x0) {
    QuickSelectTool::switchMode(this,param_4);
    return 1;
  }
  return 0;
}

// ==== Java_com_adobe_ttpixel_extension_TTPixelExtensionContextQuickSelection_reset @ 00048fa0 ====


undefined4
Java_com_adobe_ttpixel_extension_TTPixelExtensionContextQuickSelection_reset
          (int *param_1,undefined4 param_2,undefined4 param_3)

{
  QuickSelectTool *this;
  
  this = (QuickSelectTool *)
         (**(code **)(*param_1 + 0x194))(param_1,param_3,**(undefined4 **)(DAT_00048fdc + 0x48fb4));
  if (this != (QuickSelectTool *)0x0) {
    QuickSelectTool::Reset(this,0);
    return 1;
  }
  return 0;
}

// ==== Java_com_adobe_ttpixel_extension_TTPixelExtensionContextQuickSelection_setMask @ 00048fe0 ====


undefined4
Java_com_adobe_ttpixel_extension_TTPixelExtensionContextQuickSelection_setMask
          (int *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  uchar *puVar1;
  
  puVar1 = (uchar *)(**(code **)(*param_1 + 0x194))
                              (param_1,param_3,**(undefined4 **)(DAT_00049038 + 0x49000));
  if (puVar1 != (uchar *)0x0) {
    (**(code **)(*param_1 + 0x398))(param_1,param_4);
    QuickSelectTool::qsSetCurrMask(puVar1);
  }
  return 1;
}

// ==== Java_com_adobe_ttpixel_extension_TTPixelExtensionContextQuickSelection_disposeQuickSelectTool @ 0004903c ====


undefined4
Java_com_adobe_ttpixel_extension_TTPixelExtensionContextQuickSelection_disposeQuickSelectTool
          (int *param_1,undefined4 param_2,undefined4 param_3)

{
  QuickSelectTool *this;
  undefined4 *puVar1;
  
  puVar1 = *(undefined4 **)(DAT_000490ac + 0x49050);
  this = (QuickSelectTool *)(**(code **)(*param_1 + 0x194))(param_1,param_3,*puVar1);
  if (this != (QuickSelectTool *)0x0) {
    QuickSelectTool::~QuickSelectTool(this);
    operator_delete(this);
    (**(code **)(*param_1 + 0x1b8))(param_1,param_3,*puVar1,*param_1,0,0);
  }
  return 1;
}

// ==== Java_com_adobe_ttpixel_extension_TTPixelExtensionContextQuickSelection_setMaskWithARGB @ 000490b0 ====


undefined4
Java_com_adobe_ttpixel_extension_TTPixelExtensionContextQuickSelection_setMaskWithARGB
          (int *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,uint param_5)

{
  uchar *puVar1;
  int iVar2;
  void *pvVar3;
  undefined1 uVar4;
  uint uVar5;
  
  puVar1 = (uchar *)(**(code **)(*param_1 + 0x194))
                              (param_1,param_3,**(undefined4 **)(DAT_00049158 + 0x490d0));
  if (puVar1 != (uchar *)0x0) {
    iVar2 = (**(code **)(*param_1 + 0x398))(param_1,param_4);
    pvVar3 = operator_new__(param_5);
    if (param_5 != 0) {
      uVar5 = 0;
      do {
        if (*(byte *)(iVar2 + uVar5 * 4) < 0x81) {
          uVar4 = 0;
        }
        else {
          uVar4 = 0xff;
        }
        *(undefined1 *)((int)pvVar3 + uVar5) = uVar4;
        uVar5 = uVar5 + 1;
      } while (param_5 != uVar5);
    }
    QuickSelectTool::qsSetCurrMask(puVar1);
    if (pvVar3 != (void *)0x0) {
      operator_delete__(pvVar3);
    }
  }
  return 1;
}

// ==== Java_com_adobe_ttpixel_extension_TTPixelExtensionContextQuickSelection_getMask @ 0004915c ====


undefined4
Java_com_adobe_ttpixel_extension_TTPixelExtensionContextQuickSelection_getMask
          (int *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,undefined4 param_5)

{
  QuickSelectTool *this;
  int iVar1;
  int iVar2;
  undefined4 uVar3;
  
  this = (QuickSelectTool *)
         (**(code **)(*param_1 + 0x194))(param_1,param_3,**(undefined4 **)(DAT_00049210 + 0x4917c));
  if ((this == (QuickSelectTool *)0x0) || (iVar1 = QuickSelectTool::GetMask(this), iVar1 == 0)) {
    uVar3 = 0;
  }
  else {
    iVar2 = (**(code **)(*param_1 + 0x398))(param_1,param_5);
    if (iVar2 == 0) {
      uVar3 = 1;
    }
    else {
      uVar3 = (**(code **)(*param_1 + 0x39c))(param_1,param_5);
      uVar3 = FUN_00048d94(param_4,iVar1,iVar2,uVar3,*(undefined4 *)(DAT_00049214 + 0x491ec),
                           *(undefined4 *)(DAT_00049214 + 0x491f0));
    }
  }
  return uVar3;
}

// ==== Java_com_adobe_ttpixel_extension_TTPixelExtensionContextQuickSelection_mouseUp @ 00049218 ====


undefined4
Java_com_adobe_ttpixel_extension_TTPixelExtensionContextQuickSelection_mouseUp
          (int *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,undefined4 param_5)

{
  QuickSelectTool *this;
  int iVar1;
  undefined4 uVar2;
  uchar *local_1c [2];
  
  this = (QuickSelectTool *)
         (**(code **)(*param_1 + 0x194))(param_1,param_3,**(undefined4 **)(DAT_000492dc + 0x49238));
  if (this != (QuickSelectTool *)0x0) {
    local_1c[0] = (uchar *)0x0;
    QuickSelectTool::MouseUp(this,local_1c);
    if (local_1c[0] != (uchar *)0x0) {
      iVar1 = (**(code **)(*param_1 + 0x398))(param_1,param_5);
      if (iVar1 != 0) {
        uVar2 = (**(code **)(*param_1 + 0x39c))(param_1,param_5);
        uVar2 = FUN_00048d94(param_4,local_1c[0],iVar1,uVar2,*(undefined4 *)(DAT_000492e0 + 0x492b8)
                             ,*(undefined4 *)(DAT_000492e0 + 0x492bc));
        return uVar2;
      }
      return 1;
    }
  }
  return 0;
}

// ==== Java_com_adobe_ttpixel_extension_TTPixelExtensionContextQuickSelection_brushHit @ 000492e4 ====


undefined4
Java_com_adobe_ttpixel_extension_TTPixelExtensionContextQuickSelection_brushHit
          (int *param_1,undefined4 param_2,undefined4 param_3,int param_4,int param_5,int param_6,
          int param_7,undefined4 param_8)

{
  QuickSelectTool *this;
  int iVar1;
  undefined4 uVar2;
  uchar *local_1c [2];
  
  this = (QuickSelectTool *)
         (**(code **)(*param_1 + 0x194))(param_1,param_3,**(undefined4 **)(DAT_000493c0 + 0x49304));
  if (this != (QuickSelectTool *)0x0) {
    local_1c[0] = (uchar *)0x0;
    QuickSelectTool::BrushHit(this,param_4,param_5,param_7,param_6,local_1c);
    if (local_1c[0] != (uchar *)0x0) {
      iVar1 = (**(code **)(*param_1 + 0x398))(param_1,param_8);
      if (iVar1 != 0) {
        uVar2 = (**(code **)(*param_1 + 0x39c))(param_1,param_8);
        uVar2 = FUN_00048d94(0,local_1c[0],iVar1,uVar2,*(undefined4 *)(DAT_000493c4 + 0x4939c),
                             *(undefined4 *)(DAT_000493c4 + 0x493a0));
        return uVar2;
      }
      return 1;
    }
  }
  return 0;
}

// ==== Java_com_adobe_ttpixel_extension_TTPixelExtensionContextQuickSelection_qsBrushHits @ 000493c8 ====


undefined4
Java_com_adobe_ttpixel_extension_TTPixelExtensionContextQuickSelection_qsBrushHits
          (int *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,undefined4 param_5,
          int param_6,undefined4 param_7,undefined4 param_8)

{
  QuickSelectTool *this;
  int *piVar1;
  int *piVar2;
  int *piVar3;
  int iVar4;
  uint uVar5;
  undefined4 uVar6;
  uchar *local_2c [2];
  
  this = (QuickSelectTool *)
         (**(code **)(*param_1 + 0x194))(param_1,param_3,**(undefined4 **)(DAT_00049518 + 0x493dc));
  if (this != (QuickSelectTool *)0x0) {
    local_2c[0] = (uchar *)0x0;
    piVar1 = (int *)(**(code **)(*param_1 + 0x2ec))(param_1,param_4,0);
    piVar2 = (int *)(**(code **)(*param_1 + 0x2ec))(param_1,param_5,0);
    piVar3 = (int *)(**(code **)(*param_1 + 0x2ec))(param_1,param_7,0);
    iVar4 = (**(code **)(*param_1 + 0x2ac))(param_1,param_4);
    QuickSelectTool::qsBrushHit(this,piVar1,piVar2,piVar3,param_6,iVar4,local_2c);
    if (local_2c[0] != (uchar *)0x0) {
      iVar4 = (**(code **)(*param_1 + 0x398))(param_1,param_8);
      if (iVar4 != 0) {
        uVar5 = (**(code **)(*param_1 + 0x39c))(param_1,param_8);
        if ((uint)(*(int *)(DAT_0004951c + 0x494e0) * *(int *)(DAT_0004951c + 0x494e4)) <= uVar5) {
          uVar6 = FUN_00048d94(0,local_2c[0],iVar4,uVar5,*(int *)(DAT_0004951c + 0x494e0),
                               *(int *)(DAT_0004951c + 0x494e4));
          return uVar6;
        }
      }
      return 1;
    }
  }
  return 0;
}

// ==== Java_com_adobe_ttpixel_extension_TTPixelExtensionContextQuickSelection_brushHits @ 00049520 ====


undefined4
Java_com_adobe_ttpixel_extension_TTPixelExtensionContextQuickSelection_brushHits
          (int *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,undefined4 param_5,
          undefined4 param_6,undefined4 param_7,undefined4 param_8)

{
  QuickSelectTool *this;
  int *piVar1;
  int *piVar2;
  int *piVar3;
  int *piVar4;
  int iVar5;
  uint uVar6;
  undefined4 uVar7;
  uchar *local_2c [2];
  
  this = (QuickSelectTool *)
         (**(code **)(*param_1 + 0x194))(param_1,param_3,**(undefined4 **)(DAT_0004968c + 0x49534));
  if (this != (QuickSelectTool *)0x0) {
    local_2c[0] = (uchar *)0x0;
    piVar1 = (int *)(**(code **)(*param_1 + 0x2ec))(param_1,param_4,0);
    piVar2 = (int *)(**(code **)(*param_1 + 0x2ec))(param_1,param_5,0);
    piVar3 = (int *)(**(code **)(*param_1 + 0x2ec))(param_1,param_6,0);
    piVar4 = (int *)(**(code **)(*param_1 + 0x2ec))(param_1,param_7,0);
    iVar5 = (**(code **)(*param_1 + 0x2ac))(param_1,param_4);
    QuickSelectTool::BrushHit(this,piVar1,piVar2,piVar4,piVar3,iVar5,local_2c);
    if (local_2c[0] != (uchar *)0x0) {
      iVar5 = (**(code **)(*param_1 + 0x398))(param_1,param_8);
      if (iVar5 != 0) {
        uVar6 = (**(code **)(*param_1 + 0x39c))(param_1,param_8);
        if ((uint)(*(int *)(DAT_00049690 + 0x49654) * *(int *)(DAT_00049690 + 0x49658)) <= uVar6) {
          uVar7 = FUN_00048d94(0,local_2c[0],iVar5,uVar6,*(int *)(DAT_00049690 + 0x49654),
                               *(int *)(DAT_00049690 + 0x49658));
          return uVar7;
        }
      }
      return 1;
    }
  }
  return 0;
}
