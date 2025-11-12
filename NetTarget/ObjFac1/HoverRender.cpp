// HoverRender.cpp
//
// Copyright (c) 1995-1998 - Richard Langlois and Grokksoft Inc.
//
// Licensed under GrokkSoft HoverRace SourceCode License v1.0(the "License");
// you may not use this file except in compliance with the License.
//
// A copy of the license should have been attached to the package from which 
// you have taken this file. If you can not find the license you can not use 
// this file.
//
//
// The author makes no representations about the suitability of
// this software for any purpose.  It is provided "as is" "AS IS",
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
// implied.
//
// See the License for the specific language governing permissions 
// and limitations under the License.
//

#include "stdafx.h"

#include "HoverRender.h"
#include "ObjFac1Res.h"
#include "../ObjFacTools/ObjectFactoryData.h"
#include "../Util/FuzzyLogic.h"

// Local stuff

static class MR_ResActorFriend
{
   public:
      static void Draw( const MR_ResActor* pActor, MR_3DViewPort* pDest, const MR_PositionMatrix& pMatrix, int pSequence, int pFrame, const MR_Bitmap* pCockpitBitmap );
};
                    
                    





MR_HoverRender::MR_HoverRender( const MR_ObjectFromFactoryId& pId )
               :MR_MainCharacterRenderer( pId )
{
   mFrame = 0;
   
   // DEFENSIVE: Check if factory data is initialized
   if( gObjectFactoryData == NULL )
   {
      mActor0 = NULL;
      mActor1 = NULL;
      mActor2 = NULL;
      mLineCrossingSound = NULL;
      mStartSound = NULL;
      mFinishSound = NULL;
      mBumpSound = NULL;
      mJumpSound = NULL;
      mFireSound = NULL;
      mMisJumpSound = NULL;
      mMisFireSound = NULL;
      mOutOfCtrlSound = NULL;
      mMotorSound = NULL;
      mFrictionSound = NULL;
      memset( mCockpitBitmap, 0, sizeof(mCockpitBitmap) );
      memset( mCockpitBitmap2, 0, sizeof(mCockpitBitmap2) );
      return;
   }
   
   mActor0 = gObjectFactoryData->mResourceLib.GetActor( MR_ELECTRO_CAR );
   mActor1 = gObjectFactoryData->mResourceLib.GetActor( MR_HITECH_CAR );
   mActor2 = gObjectFactoryData->mResourceLib.GetActor( MR_BITURBO_CAR );

   // DEFENSIVE: Initialize sound pointers - check for null before calling GetSound()
   auto lSoundBuf = gObjectFactoryData->mResourceLib.GetShortSound( MR_SND_LINE_CROSSING );
   mLineCrossingSound = (lSoundBuf != NULL) ? lSoundBuf->GetSound() : NULL;
   
   lSoundBuf = gObjectFactoryData->mResourceLib.GetShortSound( MR_SND_START );
   mStartSound = (lSoundBuf != NULL) ? lSoundBuf->GetSound() : NULL;
   
   lSoundBuf = gObjectFactoryData->mResourceLib.GetShortSound( MR_SND_FINISH );
   mFinishSound = (lSoundBuf != NULL) ? lSoundBuf->GetSound() : NULL;
   
   lSoundBuf = gObjectFactoryData->mResourceLib.GetShortSound( MR_SND_BUMP );
   mBumpSound = (lSoundBuf != NULL) ? lSoundBuf->GetSound() : NULL;
   
   lSoundBuf = gObjectFactoryData->mResourceLib.GetShortSound( MR_SND_JUMP );
   mJumpSound = (lSoundBuf != NULL) ? lSoundBuf->GetSound() : NULL;
   
   lSoundBuf = gObjectFactoryData->mResourceLib.GetShortSound( MR_SND_FIRE );
   mFireSound = (lSoundBuf != NULL) ? lSoundBuf->GetSound() : NULL;
   
   lSoundBuf = gObjectFactoryData->mResourceLib.GetShortSound( MR_SND_MIS_JUMP );
   mMisJumpSound = (lSoundBuf != NULL) ? lSoundBuf->GetSound() : NULL;
   
   lSoundBuf = gObjectFactoryData->mResourceLib.GetShortSound( MR_SND_MIS_FIRE );
   mMisFireSound = (lSoundBuf != NULL) ? lSoundBuf->GetSound() : NULL;
   
   lSoundBuf = gObjectFactoryData->mResourceLib.GetShortSound( MR_SND_OUT_OF_CTRL );
   mOutOfCtrlSound = (lSoundBuf != NULL) ? lSoundBuf->GetSound() : NULL;
   
   auto lContSoundBuf = gObjectFactoryData->mResourceLib.GetContinuousSound( MR_SND_MOTOR );
   mMotorSound = (lContSoundBuf != NULL) ? lContSoundBuf->GetSound() : NULL;
   
   lContSoundBuf = gObjectFactoryData->mResourceLib.GetContinuousSound( MR_SND_FRICTION );
   mFrictionSound = (lContSoundBuf != NULL) ? lContSoundBuf->GetSound() : NULL;

   for( int lCounter = 0; lCounter < 10; lCounter++ )
   {
      mCockpitBitmap[ lCounter ] = gObjectFactoryData->mResourceLib.GetBitmap( MR_CAR_COCKPIT1+lCounter );
      mCockpitBitmap2[ lCounter ] = gObjectFactoryData->mResourceLib.GetBitmap( MR_CAR_COCKPIT21+lCounter );
   }

}

MR_HoverRender::~MR_HoverRender()
{

}

void MR_HoverRender::Render( MR_3DViewPort* pDest, 
                             const MR_3DCoordinate& pPosition, 
                             MR_Angle pOrientation,
                             BOOL     pMotorOn, 
                             int      pHoverId,
                             int      pModel        )
{
   // DEFENSIVE: Validate all input parameters
   if( pDest == NULL ) return;
   if( pModel < 0 || pModel > 2 ) return;  // Invalid model ID

   // Compute the required rotation matrix
   MR_PositionMatrix lMatrix;

   if( pDest->ComputePositionMatrix( lMatrix, pPosition, pOrientation, 1000 /* TODO Object ray must be precomputed at compilation*/ ) )
   {
      int lSeq   = pMotorOn?1:0;

      if( pMotorOn )
      {
         mFrame = (mFrame+1)%2;
      }
      else
      {
         mFrame =0;
      }

      // DEFENSIVE: Select actor with null checks
      const MR_ResActor* lActor = NULL;
      if( pModel == 1 )
      {
         lActor = mActor1;
      }
      else if( pModel == 2 )
      {
         lActor = mActor2;
      }
      else
      {
         lActor = mActor0;
      }

      // DEBUG: Log render calls to diagnose coloring issue
      static int render_count = 0;
      FILE* logFile = fopen("C:\\originalhr\\HoverRace\\Release\\Game2_HoverRender.log", "a");
      if( logFile && (render_count % 60 == 0) )  // Log every 60th call
      {
         fprintf(logFile, "[Render #%d] Model=%d, Motor=%s, HoverId=%d, Actor=%p, Draw=%s\n",
            render_count, pModel, pMotorOn ? "ON" : "OFF", pHoverId,
            lActor, lActor != NULL ? "YES" : "NO");
         
         if( lActor == NULL )
         {
            fprintf(logFile, "  WARNING: Actor is NULL! mActor0=%p, mActor1=%p, mActor2=%p\n",
               mActor0, mActor1, mActor2);
            fprintf(logFile, "  gObjectFactoryData=%p\n", gObjectFactoryData);
         }
         fflush(logFile);
         fclose(logFile);
      }
      render_count++;

      // DEFENSIVE: Only draw if actor is valid
      if( lActor != NULL )
      {
         const MR_Bitmap* lCockpitBitmap = NULL;
         if( pModel == 1 )
         {
            lCockpitBitmap = mCockpitBitmap2[ pHoverId%10 ];
         }
         else
         {
            lCockpitBitmap = mCockpitBitmap[ pHoverId%10 ];
         }

         MR_ResActorFriend::Draw( lActor, pDest, lMatrix, lSeq, mFrame, lCockpitBitmap );
      }
   } 
}   

MR_ShortSound* MR_HoverRender::GetLineCrossingSound()
{
   return mLineCrossingSound;
}

MR_ShortSound* MR_HoverRender::GetStartSound()
{
   return mStartSound;
}

MR_ShortSound* MR_HoverRender::GetFinishSound()
{
   return mFinishSound;
}

MR_ShortSound* MR_HoverRender::GetBumpSound()
{
   return mBumpSound;
}

MR_ShortSound* MR_HoverRender::GetJumpSound()
{
   return mJumpSound;
}

MR_ShortSound* MR_HoverRender::GetFireSound()
{
   return mFireSound;
}

MR_ShortSound* MR_HoverRender::GetMisJumpSound()
{
   return mMisJumpSound;
}

MR_ShortSound* MR_HoverRender::GetMisFireSound()
{
   return mMisFireSound;
}

MR_ShortSound* MR_HoverRender::GetOutOfCtrlSound()
{
   return mOutOfCtrlSound;
}

MR_ContinuousSound* MR_HoverRender::GetMotorSound()
{
   return mMotorSound;
}

MR_ContinuousSound* MR_HoverRender::GetFrictionSound()
{
   return mFrictionSound;
}


void MR_ResActorFriend::Draw( const MR_ResActor* pActor, MR_3DViewPort* pDest, const MR_PositionMatrix& pMatrix, int pSequence, int pFrame, const MR_Bitmap* pCockpitBitmap )
{
   // DEFENSIVE: Validate all pointers and parameters
   if( pActor == NULL ) return;
   if( pDest == NULL ) return;
   if( pSequence < 0 || pSequence >= 2 ) return;  // Only 2 sequences (0=off, 1=on)
   if( pFrame < 0 || pFrame >= 2 ) return;  // Only 2 frames per sequence

   // DEFENSIVE: Validate sequence list access
   if( pActor->mSequenceList == NULL ) return;
   
   MR_ResActor::Frame* lFrame = &(pActor->mSequenceList[ pSequence ].mFrameList[ pFrame ]);

   if( lFrame == NULL ) return;  // Frame is invalid
   if( lFrame->mComponentList == NULL ) return;  // No components to render
   if( lFrame->mNbComponent <= 0 ) return;  // No components

   for( int lCounter = 0; lCounter < lFrame->mNbComponent; lCounter++ )
   {
      // DEFENSIVE: Validate array access
      if( lCounter < 0 || lCounter >= lFrame->mNbComponent ) break;
      
      MR_ResActor::Patch* lPatch = (MR_ResActor::Patch*)lFrame->mComponentList[ lCounter ];

      if( lPatch == NULL ) continue;  // Skip NULL patches
      if( lPatch->mBitmap == NULL ) continue;  // Skip patches with no bitmap

      int lBitmapResId = lPatch->mBitmap->GetResourceId();

      if( (lBitmapResId == MR_CAR_COCKPIT)||(lBitmapResId == MR_CAR2_COCKPIT)||(lBitmapResId == MR_ECAR_COCKPIT) )
      {
         if( pCockpitBitmap != NULL )
         {
            pDest->RenderPatch( *lPatch, pMatrix , pCockpitBitmap );
         }
      }
      else
      {
         pDest->RenderPatch( *lPatch, pMatrix , lPatch->mBitmap );
      }
   }
}
