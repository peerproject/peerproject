#include "rar.hpp"

RAROptions::RAROptions()
{
  Init();
}


RAROptions::~RAROptions()
{
  // It is important for security reasons, so we do not have the unnecessary password data left in memory.
  memset(this,0,sizeof(RAROptions));
}


void RAROptions::Init()
{
  memset(this,0,sizeof(RAROptions));
  WinSize=0x400000;
  Overwrite=OVERWRITE_DEFAULT;
  Method=3;
  MsgStream=MSG_STDOUT;
  ConvertNames=NAMES_ORIGINALCASE;
  ProcessEA=true;
  xmtime=EXTTIME_HIGH3;
  CurVolNum=0;
  FileSizeLess=INT64NDF;
  FileSizeMore=INT64NDF;
}
