// GAME.h
//
//////////////////////////////////////////////////////////////////////


#if !defined(AFX_GAMELIB_H__B628EDC0_7542_4E57_9003_82D1A3DFD6FC__INCLUDED_)
#define AFX_GAMELIB_H__B628EDC0_7542_4E57_9003_82D1A3DFD6FC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef WIN32
  #pragma warning ( disable : 4786 )
#endif

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <list>
#include <vector>
#include <map>
#include <deque>
#include <string>

#define YAW		0
#define PITCH	1
#define ROLL	2

#include "GameRunTimeLib.h"
#include "VectorLib.h"

using namespace std;
#ifndef GAME_LIB_PROYECT
    #include "ResourceStore.h"
    #include "GameSpecs.h"
#endif
#endif // !defined(AFX_VECTORLIB_H__B628EDC0_7542_4E57_9003_82D1A3DFD6FC__INCLUDED_)
