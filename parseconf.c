//
// Created by S on 2019/5/15.
//

//#include <bits/types/FILE.h>
#include "parseconf.h"
#include "tunable.h"
#include "common.h"

void parseconf_load_file (const char *path)
{
  FILE *fp = fopen (path, "r");
  if (fp == NULL)
    {
      ERR_EXIT ("fopen");
    }

    char setting_line[1024]={0};
  while(fgets(setting_line,sizeof(setting_line),fp)!=NULL)
  {

  }

}

void paresconf_load_setting (const char *setting)
{

}