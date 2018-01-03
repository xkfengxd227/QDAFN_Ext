// -----------------------------------------------------------------------------
//  Copyright (c) 2013-2016 Sun Yat-Sen University (SYSU). All Rights Reserved.
//
//  SYSU grants permission to use, copy, modify, and distribute this software
//  and its documentation for NON-COMMERCIAL purposes and without fee, provided
//  that this copyright notice appears in all copies.
//
//  SYSU provides this software "as is," without representations or warranties
//  of any kind, either expressed or implied, including but not limited to the 
//  implied warranties of merchantability, fitness for a particular purpose, 
//  and noninfringement. SIST shall not be liable for any damages arising from 
//  any use of this software.
//
// Authors: Qiang Huang  (huangq2011@gmail.com)
//
// Created on:       2015-02-08
// Last Modified on: 2016-04-12
// Version 0.2.1
// -----------------------------------------------------------------------------

#include <stdio.h>
#include <errno.h>

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <vector>
#include <queue>
#include <algorithm>

// -----------------------------------------------------------------------------
//  For Linux directory
// -----------------------------------------------------------------------------
#include <unistd.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/types.h>
#define  LINUX_

// -----------------------------------------------------------------------------
//  For Windows directory
// -----------------------------------------------------------------------------
//#include <direct.h>
//#include <io.h>

#include "def.h"
#include "pri_queue.h"
#include "util.h"
#include "block_file.h"
#include "b_node.h"
#include "b_tree.h"
#include "qdafn.h"
#include "afn.h"

using namespace std;