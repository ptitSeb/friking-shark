#include "./stdafx.h"
#include "EntityUtilities.h"

double ApproachAngle(double actual,double ideal, double ammount)
{
    double result;

    /*addammount=0;
    destRespectOrig=ideal-actual;
    if(destRespectOrig<0){destRespectOrig=destRespectOrig+360;}

    if(destRespectOrig>=0 && destRespectOrig<180)
    {addammount=0;}else{addammount=1;}
*/
  /*  // if almost in perfect back take a random direction (it gets nicer)
    if(destRespectOrig>(180-ammount/2) && destRespectOrig<(180+ammount/2))
    {
        if(drand()<=0.5){addammount=1;}else{addammount=0;}
    }*/
  //  if(addammount)
    //{
        if(fabs(actual-ideal)<ammount)
        {result=ideal;}
        else
        {result=actual+ammount;}
  /*    }
 // else
   // {
        if(fabs(actual-ideal)<ammount)
        {result=ideal;}
     //   else
     //   {result=actual-ammount;}
  //  }*/
    if(result<0){result=result+360;}
    if(result>360){result=result+(-360);}
    return result;
}

void ApproachAngles(CVector *pvCurrent,CVector &vIdeal, double ammount)
{
    pvCurrent->c[YAW]=ApproachAngle(pvCurrent->c[YAW],vIdeal.c[YAW],ammount);
    pvCurrent->c[PITCH]=ApproachAngle(pvCurrent->c[PITCH],vIdeal.c[PITCH],ammount);
}