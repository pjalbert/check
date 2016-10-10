/*
 *  Command.c
 *  Idiome
 *
 *  Created by Pierre-Jean ALBERT on Tue Apr 08 2014.
 *  Copyright (c) 2013 __An Intellectual Projet__. All rights reserved.
 *
 */

#include "command.h"

/**
		 */
int main (int argn, char* argc[]) {
  CentralExec* pCentralExec;

  pCentralExec = (CentralExec*)InitCentralExec("AIP%");
  if (!pCentralExec)
  {
    printf("Error Creating CentralExec - \n");
    exit(-1);
  }

	LoadDefaultCommand(pCentralExec);

  RunCentralExec(pCentralExec);

  FreeCentralExec(pCentralExec);

  return 0;
}
