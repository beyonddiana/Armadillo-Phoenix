/** 
 * @file llpanelobject.cpp
 * @brief Object editing (position, scale, etc.) in the tools floater
 *
 * $LicenseInfo:firstyear=2001&license=viewerlgpl$
 * Second Life Viewer Source Code
 * Copyright (C) 2010, Linden Research, Inc.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation;
 * version 2.1 of the License only.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 * 
 * Linden Research, Inc., 945 Battery Street, San Francisco, CA  94111  USA
 * $/LicenseInfo$
 */

#include "llviewerprecompiledheaders.h"

// file include
#include "llpanelobject.h"

// linden library includes
#include "lleconomy.h"
#include "llerror.h"
#include "llfontgl.h"
#include "llpermissionsflags.h"
#include "llstring.h"
#include "llvolume.h"
#include "m3math.h"

// project includes
#include "llagent.h"
#include "llbutton.h"
#include "llcalc.h"
#include "llcheckboxctrl.h"
#include "llcolorswatch.h"
#include "llcombobox.h"
#include "llfocusmgr.h"
#include "llmanipscale.h"
#include "llpreviewscript.h"
#include "llresmgr.h"
#include "llselectmgr.h"
#include "llspinctrl.h"
#include "lltexturectrl.h"
#include "lltextbox.h"
#include "lltool.h"
#include "lltoolcomp.h"
#include "lltoolmgr.h"
#include "llui.h"
#include "llviewerobject.h"
#include "llviewerregion.h"
#include "llviewerwindow.h"
#include "llvovolume.h"
#include "llworld.h"
#include "pipeline.h"
#include "llviewercontrol.h"
#include "lluictrlfactory.h"
//#include "llfirstuse.h"
// [RLVa:KB] - Checked: 2011-05-22 (RLVa-1.3.1a)
#include "rlvhandler.h"
#include "llvoavatarself.h"
// [/RLVa:KB]

#include "llwindow.h" // for clipboad operations -KC

// [RLVa:KB] - Checked: 2011-05-22 (RLVa-1.3.1a)
#include "rlvhandler.h"
#include "llvoavatarself.h"
// [/RLVa:KB]
#include "llinventoryfunctions.h"
#include "llinventorymodel.h"

#include "lldrawpool.h"

//
// Constants
//
enum {
	MI_BOX,
	MI_CYLINDER,
	MI_PRISM,
	MI_SPHERE,
	MI_TORUS,
	MI_TUBE,
	MI_RING,
	MI_SCULPT,
	MI_PATH_LINE_PROFILE_CIRCLE_HALF,
	MI_PATH_CIRCLE_PROFILE_CIRCLE_HALF,
	MI_PATH_CIRCLE2_PROFILE_SQUARE,
	MI_PATH_CIRCLE2_PROFILE_TRI,
	MI_PATH_CIRCLE2_PROFILE_CIRCLE,
	MI_PATH_CIRCLE2_PROFILE_CIRCLE_HALF,
	MI_PATH_TEST_PROFILE_SQUARE,
	MI_PATH_TEST_PROFILE_TRI,
	MI_PATH_TEST_PROFILE_CIRCLE,
	MI_PATH_TEST_PROFILE_CIRCLE_HALF,
	//<-- Working33 by Gregory Maurer
	MI_PATH_33_PROFILE_CIRCLE,
	MI_PATH_33_PROFILE_SQUARE,
	MI_PATH_33_PROFILE_TRIANGLE,
	MI_PATH_33_PROFILE_HALFCIRCLE,
	//Working33 -->
	MI_NONE,
	MI_VOLUME_COUNT
};

enum {
	MI_HOLE_SAME,
	MI_HOLE_CIRCLE,
	MI_HOLE_SQUARE,
	MI_HOLE_TRIANGLE,
	MI_HOLE_COUNT
};

//static const std::string LEGACY_FULLBRIGHT_DESC =LLTrans::getString("Fullbright");

BOOL	LLPanelObject::postBuild()
{
	setMouseOpaque(FALSE);
	
	//--------------------------------------------------------
	// Top
	//--------------------------------------------------------
	
	// Lock checkbox
	mCheckLock = getChild<LLCheckBoxCtrl>("checkbox locked");
	childSetCommitCallback("checkbox locked",onCommitLock,this);

	// Physical checkbox
	mCheckPhysics = getChild<LLCheckBoxCtrl>("Physical Checkbox Ctrl");
	childSetCommitCallback("Physical Checkbox Ctrl",onCommitPhysics,this);

	// Temporary checkbox
	mCheckTemporary = getChild<LLCheckBoxCtrl>("Temporary Checkbox Ctrl");
	childSetCommitCallback("Temporary Checkbox Ctrl",onCommitTemporary,this);

	// Phantom checkbox
	mCheckPhantom = getChild<LLCheckBoxCtrl>("Phantom Checkbox Ctrl");
	childSetCommitCallback("Phantom Checkbox Ctrl",onCommitPhantom,this);
       
	// Position
	mLabelPosition = getChild<LLTextBox>("label position");
	mCtrlPosX = getChild<LLSpinCtrl>("Pos X");
	childSetCommitCallback("Pos X",onCommitPosition,this);
	mCtrlPosY = getChild<LLSpinCtrl>("Pos Y");
	childSetCommitCallback("Pos Y",onCommitPosition,this);
	mCtrlPosZ = getChild<LLSpinCtrl>("Pos Z");
	childSetCommitCallback("Pos Z",onCommitPosition,this);

	// Scale
	mLabelSize = getChild<LLTextBox>("label size");
	mCtrlScaleX = getChild<LLSpinCtrl>("Scale X");
	childSetCommitCallback("Scale X",onCommitScale,this);

	// Scale Y
	mCtrlScaleY = getChild<LLSpinCtrl>("Scale Y");
	childSetCommitCallback("Scale Y",onCommitScale,this);

	// Scale Z
	mCtrlScaleZ = getChild<LLSpinCtrl>("Scale Z");
	childSetCommitCallback("Scale Z",onCommitScale,this);

	// Rotation
	mLabelRotation = getChild<LLTextBox>("label rotation");
	mCtrlRotX = getChild<LLSpinCtrl>("Rot X");
	childSetCommitCallback("Rot X",onCommitRotation,this);
	mCtrlRotY = getChild<LLSpinCtrl>("Rot Y");
	childSetCommitCallback("Rot Y",onCommitRotation,this);
	mCtrlRotZ = getChild<LLSpinCtrl>("Rot Z");
	childSetCommitCallback("Rot Z",onCommitRotation,this);

	// Copy/paste pos
	mBtnCopyPos = getChild<LLButton>("copypos");
	mBtnCopyPos->setCommitCallback( boost::bind(&LLPanelObject::onCopyPos, this, _2 ));
	mBtnPastePos = getChild<LLButton>("pastepos");
	mBtnPastePos->setCommitCallback( boost::bind(&LLPanelObject::onPastePos, this, _2 ));
	mBtnPastePosClip = getChild<LLButton>("pasteposclip");
	mBtnPastePosClip->setCommitCallback( boost::bind(&LLPanelObject::onPastePosClip, this, _2 ));

	// Copy/paste size
	mBtnCopySize = getChild<LLButton>("copysize");
	mBtnCopySize->setCommitCallback( boost::bind(&LLPanelObject::onCopySize, this, _2 ));
	mBtnPasteSize = getChild<LLButton>("pastesize");
	mBtnPasteSize->setCommitCallback( boost::bind(&LLPanelObject::onPasteSize, this, _2 ));
	mBtnPasteSizeClip = getChild<LLButton>("pastesizeclip");
	mBtnPasteSizeClip->setCommitCallback( boost::bind(&LLPanelObject::onPasteSizeClip, this, _2 ));

	// Copy/paste rot
	mBtnCopyRot = getChild<LLButton>("copyrot");
	mBtnCopyRot->setCommitCallback( boost::bind(&LLPanelObject::onCopyRot, this, _2 ));
	mBtnPasteRot = getChild<LLButton>("pasterot");
	mBtnPasteRot->setCommitCallback( boost::bind(&LLPanelObject::onPasteRot, this, _2 ));
	mBtnPasteRotClip = getChild<LLButton>("pasterotclip");
	mBtnPasteRotClip->setCommitCallback( boost::bind(&LLPanelObject::onPasteRotClip, this, _2 ));

	// Copy/paste obj prams
	mBtnCopyParams = getChild<LLButton>("copyparams");
	mBtnCopyParams->setCommitCallback( boost::bind(&LLPanelObject::onCopyParams, this, _2 ));
	mBtnPasteParams = getChild<LLButton>("pasteparams");
	mBtnPasteParams->setCommitCallback( boost::bind(&LLPanelObject::onPasteParams, this, _2 ));

	//--------------------------------------------------------
		
	// Base Type
	mComboBaseType = getChild<LLComboBox>("comboBaseType");
	childSetCommitCallback("comboBaseType",onCommitParametric,this);

	// Cut
	mLabelCut = getChild<LLTextBox>("text cut");
	mSpinCutBegin = getChild<LLSpinCtrl>("cut begin");
	childSetCommitCallback("cut begin",onCommitParametric,this);
	mSpinCutBegin->setValidateBeforeCommit( precommitValidate );
	mSpinCutEnd = getChild<LLSpinCtrl>("cut end");
	childSetCommitCallback("cut end",onCommitParametric,this);
	mSpinCutEnd->setValidateBeforeCommit( &precommitValidate );

	// Hollow / Skew
	mLabelHollow = getChild<LLTextBox>("text hollow");
	mLabelSkew = getChild<LLTextBox>("text skew");
	mSpinHollow = getChild<LLSpinCtrl>("Scale 1");
	childSetCommitCallback("Scale 1",onCommitParametric,this);
	mSpinHollow->setValidateBeforeCommit( &precommitValidate );
	mSpinSkew = getChild<LLSpinCtrl>("Skew");
	childSetCommitCallback("Skew",onCommitParametric,this);
	mSpinSkew->setValidateBeforeCommit( &precommitValidate );
	mLabelHoleType = getChild<LLTextBox>("Hollow Shape");

	// Hole Type
	mComboHoleType = getChild<LLComboBox>("hole");
	childSetCommitCallback("hole",onCommitParametric,this);

	// Twist
	mLabelTwist = getChild<LLTextBox>("text twist");
	mSpinTwistBegin = getChild<LLSpinCtrl>("Twist Begin");
	childSetCommitCallback("Twist Begin",onCommitParametric,this);
	mSpinTwistBegin->setValidateBeforeCommit( precommitValidate );
	mSpinTwist = getChild<LLSpinCtrl>("Twist End");
	childSetCommitCallback("Twist End",onCommitParametric,this);
	mSpinTwist->setValidateBeforeCommit( &precommitValidate );

	// Scale
	mSpinScaleX = getChild<LLSpinCtrl>("Taper Scale X");
	childSetCommitCallback("Taper Scale X",onCommitParametric,this);
	mSpinScaleX->setValidateBeforeCommit( &precommitValidate );
	mSpinScaleY = getChild<LLSpinCtrl>("Taper Scale Y");
	childSetCommitCallback("Taper Scale Y",onCommitParametric,this);
	mSpinScaleY->setValidateBeforeCommit( &precommitValidate );

	// Shear
	mLabelShear = getChild<LLTextBox>("text topshear");
	mSpinShearX = getChild<LLSpinCtrl>("Shear X");
	childSetCommitCallback("Shear X",onCommitParametric,this);
	mSpinShearX->setValidateBeforeCommit( &precommitValidate );
	mSpinShearY = getChild<LLSpinCtrl>("Shear Y");
	childSetCommitCallback("Shear Y",onCommitParametric,this);
	mSpinShearY->setValidateBeforeCommit( &precommitValidate );

	// Path / Profile
	mCtrlPathBegin = getChild<LLSpinCtrl>("Path Limit Begin");
	childSetCommitCallback("Path Limit Begin",onCommitParametric,this);
	mCtrlPathBegin->setValidateBeforeCommit( &precommitValidate );
	mCtrlPathEnd = getChild<LLSpinCtrl>("Path Limit End");
	childSetCommitCallback("Path Limit End",onCommitParametric,this);
	mCtrlPathEnd->setValidateBeforeCommit( &precommitValidate );

	// Taper
	mLabelTaper = getChild<LLTextBox>("text taper2");
	mSpinTaperX = getChild<LLSpinCtrl>("Taper X");
	childSetCommitCallback("Taper X",onCommitParametric,this);
	mSpinTaperX->setValidateBeforeCommit( precommitValidate );
	mSpinTaperY = getChild<LLSpinCtrl>("Taper Y");
	childSetCommitCallback("Taper Y",onCommitParametric,this);
	mSpinTaperY->setValidateBeforeCommit( precommitValidate );
	
	// Radius Offset / Revolutions
	mLabelRadiusOffset = getChild<LLTextBox>("text radius delta");
	mLabelRevolutions = getChild<LLTextBox>("text revolutions");
	mSpinRadiusOffset = getChild<LLSpinCtrl>("Radius Offset");
	childSetCommitCallback("Radius Offset",onCommitParametric,this);
	mSpinRadiusOffset->setValidateBeforeCommit( &precommitValidate );
	mSpinRevolutions = getChild<LLSpinCtrl>("Revolutions");
	childSetCommitCallback("Revolutions",onCommitParametric,this);
	mSpinRevolutions->setValidateBeforeCommit( &precommitValidate );

	// Sculpt
	mCtrlSculptTexture = getChild<LLTextureCtrl>("sculpt texture control");
	if (mCtrlSculptTexture)
	{
		mCtrlSculptTexture->setDefaultImageAssetID(LLUUID(SCULPT_DEFAULT_TEXTURE));
		mCtrlSculptTexture->setCommitCallback( boost::bind(&LLPanelObject::onCommitSculpt, this, _2 ));
		mCtrlSculptTexture->setOnCancelCallback( boost::bind(&LLPanelObject::onCancelSculpt, this, _2 ));
		mCtrlSculptTexture->setOnSelectCallback( boost::bind(&LLPanelObject::onSelectSculpt, this, _2 ));
		mCtrlSculptTexture->setDropCallback( boost::bind(&LLPanelObject::onDropSculpt, this, _2 ));
		// Don't allow (no copy) or (no transfer) textures to be selected during immediate mode
		mCtrlSculptTexture->setImmediateFilterPermMask(PERM_COPY | PERM_TRANSFER);
		mCtrlSculptTexture->setDnDFilterPermMask(PERM_COPY | PERM_TRANSFER);
		// Allow any texture to be used during non-immediate mode.
		mCtrlSculptTexture->setNonImmediateFilterPermMask(PERM_NONE);
		LLAggregatePermissions texture_perms;
		if (LLSelectMgr::getInstance()->selectGetAggregateTexturePermissions(texture_perms))
		{
			BOOL can_copy =
				texture_perms.getValue(PERM_COPY) == LLAggregatePermissions::AP_EMPTY ||
				texture_perms.getValue(PERM_COPY) == LLAggregatePermissions::AP_ALL;
			BOOL can_transfer =
				texture_perms.getValue(PERM_TRANSFER) == LLAggregatePermissions::AP_EMPTY ||
				texture_perms.getValue(PERM_TRANSFER) == LLAggregatePermissions::AP_ALL;
			mCtrlSculptTexture->setCanApplyImmediately(can_copy && can_transfer);
		}
		else
		{
			mCtrlSculptTexture->setCanApplyImmediately(FALSE);
		}
	}

	mLabelSculptType = getChild<LLTextBox>("label sculpt type");
	mCtrlSculptType = getChild<LLComboBox>("sculpt type control");
	childSetCommitCallback("sculpt type control", onCommitSculptType, this);
	mCtrlSculptMirror = getChild<LLCheckBoxCtrl>("sculpt mirror control");
	childSetCommitCallback("sculpt mirror control", onCommitSculptType, this);
	mCtrlSculptInvert = getChild<LLCheckBoxCtrl>("sculpt invert control");
	childSetCommitCallback("sculpt invert control", onCommitSculptType, this);

	// Start with everyone disabled
	clearCtrls();

// <FS:CR> Aurora Sim
	updateLimits(FALSE);	// default to non-attachment
// </FS:CR> Aurora Sim

	return TRUE;
}

LLPanelObject::LLPanelObject()
:	LLPanel(),
	mIsPhysical(FALSE),
	mIsTemporary(FALSE),
	mIsPhantom(FALSE),
	mSelectedType(MI_BOX),
	mSculptTextureRevert(LLUUID::null),
	mSculptTypeRevert(0),
	mHasPosClipboard(FALSE),
	mHasSizeClipboard(FALSE),
	mHasRotClipboard(FALSE),
	mHasParamClipboard(FALSE),
	mHasFlexiParam(FALSE),
	mHasSculptParam(FALSE),
	mHasLightParam(FALSE)
{
}


LLPanelObject::~LLPanelObject()
{
	// Children all cleaned up by default view destructor.
}

// <AW: opensim-limits>
void LLPanelObject::updateLimits(BOOL attachment)
{
// <FS:CR> Aurora Sim
	//mRegionMaxHeight = LLWorld::getInstance()->getRegionMaxHeight();
	//mCtrlPosZ->setMaxValue(mRegionMaxHeight);
//<FS:TS> FIRE-8205: Unable to edit attachments to negative coordinates
//  Only apply region size limits to position spinners if editing something
//    that's not an attachment. Attachments have their own limits.
	if (attachment)
	{
		mCtrlPosX->setMinValue(-MAX_ATTACHMENT_DIST);
		mCtrlPosX->setMaxValue(MAX_ATTACHMENT_DIST);
		mCtrlPosY->setMinValue(-MAX_ATTACHMENT_DIST);
		mCtrlPosY->setMaxValue(MAX_ATTACHMENT_DIST);
		mCtrlPosZ->setMinValue(-MAX_ATTACHMENT_DIST);
		mCtrlPosZ->setMaxValue(MAX_ATTACHMENT_DIST);
	}
	else
	{
		mCtrlPosX->setMinValue(LLWorld::getInstance()->getMinPrimXPos());
		mCtrlPosX->setMaxValue(LLWorld::getInstance()->getMaxPrimXPos());
		mCtrlPosY->setMinValue(LLWorld::getInstance()->getMinPrimYPos());
		mCtrlPosY->setMaxValue(LLWorld::getInstance()->getMaxPrimYPos());
		mCtrlPosZ->setMinValue(LLWorld::getInstance()->getMinPrimZPos());
		mCtrlPosZ->setMaxValue(LLWorld::getInstance()->getMaxPrimZPos());
	}
//</FS:TS> FIRE-8205: Unable to edit attachments to negative coordinates
// </FS:CR> Aurora Sim
	mMinScale = LLWorld::getInstance()->getRegionMinPrimScale();
	mMaxScale = LLWorld::getInstance()->getRegionMaxPrimScale();
	mCtrlScaleX->setMinValue(mMinScale);
	mCtrlScaleX->setMaxValue(mMaxScale);
	mCtrlScaleY->setMinValue(mMinScale);
	mCtrlScaleY->setMaxValue(mMaxScale);
	mCtrlScaleZ->setMinValue(mMinScale);
	mCtrlScaleZ->setMaxValue(mMaxScale);

	mMaxHollowSize = LLWorld::getInstance()->getRegionMaxHollowSize();
	mSpinHollow->setMaxValue(mMaxHollowSize);

	mMinHoleSize = LLWorld::getInstance()->getRegionMinHoleSize();
	mSpinScaleX->setMinValue(mMinHoleSize);
	mSpinScaleY->setMinValue(mMinHoleSize);
// <FS:CR> Aurora Sim
	mCheckPhysics->setEnabled(LLWorld::getInstance()->getAllowPhysicalPrims());
// </FS:CR> Aurora Sim
}
// </AW: opensim-limits>

void LLPanelObject::getState( )
{
	LLViewerObject* objectp = LLSelectMgr::getInstance()->getSelection()->getFirstRootObject();
	LLViewerObject* root_objectp = objectp;
	if(!objectp)
	{
		objectp = LLSelectMgr::getInstance()->getSelection()->getFirstObject();
		// *FIX: shouldn't we just keep the child?
		if (objectp)
		{
			LLViewerObject* parentp = objectp->getRootEdit();

			if (parentp)
			{
				root_objectp = parentp;
			}
			else
			{
				root_objectp = objectp;
			}
		}
	}

	LLCalc* calcp = LLCalc::getInstance();

	LLVOVolume *volobjp = NULL;
	if ( objectp && (objectp->getPCode() == LL_PCODE_VOLUME))
	{
		volobjp = (LLVOVolume *)objectp;
	}

	if( !objectp )
	{
		//forfeit focus
		if (gFocusMgr.childHasKeyboardFocus(this))
		{
			gFocusMgr.setKeyboardFocus(NULL);
		}

		// Disable all text input fields
		clearCtrls();
		calcp->clearAllVariables();
		return;
	}

	// can move or rotate only linked group with move permissions, or sub-object with move and modify perms
	BOOL enable_move	= objectp->permMove() && !objectp->isPermanentEnforced() && ((root_objectp == NULL) || !root_objectp->isPermanentEnforced()) && (objectp->permModify() || !gSavedSettings.getBOOL("EditLinkedParts"));
	BOOL enable_scale	= objectp->permMove() && !objectp->isPermanentEnforced() && ((root_objectp == NULL) || !root_objectp->isPermanentEnforced()) && objectp->permModify();
	BOOL enable_rotate	= objectp->permMove() && !objectp->isPermanentEnforced() && ((root_objectp == NULL) || !root_objectp->isPermanentEnforced()) && (objectp->permModify() || !gSavedSettings.getBOOL("EditLinkedParts"));

	S32 selected_count = LLSelectMgr::getInstance()->getSelection()->getObjectCount();
	BOOL single_volume = (LLSelectMgr::getInstance()->selectionAllPCode( LL_PCODE_VOLUME ))
						 && (selected_count == 1);

//<FS:TS> FIRE-8205: Unable to edit attachment to negative coordinates
//  Update the position limits depending on whether this is an attachment.
	updateLimits(objectp->isAttachment());
//</FS:TS> FIRE-8205

	if (LLSelectMgr::getInstance()->getSelection()->getRootObjectCount() > 1)
	{
		enable_move = FALSE;
		enable_scale = FALSE;
		enable_rotate = FALSE;
	}

// [RLVa:KB] - Checked: 2010-03-31 (RLVa-1.2.0c) | Modified: RLVa-1.0.0g
	if ( (rlv_handler_t::isEnabled()) && ((gRlvHandler.hasBehaviour(RLV_BHVR_UNSIT)) || (gRlvHandler.hasBehaviour(RLV_BHVR_SITTP))) )
	{
		if ( (isAgentAvatarValid()) && (gAgentAvatarp->isSitting()) && (gAgentAvatarp->getRoot() == objectp->getRootEdit()) )
			enable_move = enable_scale = enable_rotate = FALSE;
	}
// [/RLVa:KB]

	LLVector3 vec;
	if (enable_move)
	{
		vec = objectp->getPositionEdit();
		mCtrlPosX->set( vec.mV[VX] );
		mCtrlPosY->set( vec.mV[VY] );
		mCtrlPosZ->set( vec.mV[VZ] );
		calcp->setVar(LLCalc::X_POS, vec.mV[VX]);
		calcp->setVar(LLCalc::Y_POS, vec.mV[VY]);
		calcp->setVar(LLCalc::Z_POS, vec.mV[VZ]);
	}
	else
	{
		mCtrlPosX->clear();
		mCtrlPosY->clear();
		mCtrlPosZ->clear();
		calcp->clearVar(LLCalc::X_POS);
		calcp->clearVar(LLCalc::Y_POS);
		calcp->clearVar(LLCalc::Z_POS);
	}


	mLabelPosition->setEnabled( enable_move );
	mCtrlPosX->setEnabled(enable_move);
	mCtrlPosY->setEnabled(enable_move);
	mCtrlPosZ->setEnabled(enable_move);

	if (enable_scale)
	{
		vec = objectp->getScale();
		mCtrlScaleX->set( vec.mV[VX] );
		mCtrlScaleY->set( vec.mV[VY] );
		mCtrlScaleZ->set( vec.mV[VZ] );
		calcp->setVar(LLCalc::X_SCALE, vec.mV[VX]);
		calcp->setVar(LLCalc::Y_SCALE, vec.mV[VY]);
		calcp->setVar(LLCalc::Z_SCALE, vec.mV[VZ]);
	}
	else
	{
		mCtrlScaleX->clear();
		mCtrlScaleY->clear();
		mCtrlScaleZ->clear();
		calcp->setVar(LLCalc::X_SCALE, 0.f);
		calcp->setVar(LLCalc::Y_SCALE, 0.f);
		calcp->setVar(LLCalc::Z_SCALE, 0.f);
	}

	mLabelSize->setEnabled( enable_scale );
	mCtrlScaleX->setEnabled( enable_scale );
	mCtrlScaleY->setEnabled( enable_scale );
	mCtrlScaleZ->setEnabled( enable_scale );

	LLQuaternion object_rot = objectp->getRotationEdit();
	object_rot.getEulerAngles(&(mCurEulerDegrees.mV[VX]), &(mCurEulerDegrees.mV[VY]), &(mCurEulerDegrees.mV[VZ]));
	mCurEulerDegrees *= RAD_TO_DEG;
	mCurEulerDegrees.mV[VX] = fmod(llround(mCurEulerDegrees.mV[VX], OBJECT_ROTATION_PRECISION) + 360.f, 360.f);
	mCurEulerDegrees.mV[VY] = fmod(llround(mCurEulerDegrees.mV[VY], OBJECT_ROTATION_PRECISION) + 360.f, 360.f);
	mCurEulerDegrees.mV[VZ] = fmod(llround(mCurEulerDegrees.mV[VZ], OBJECT_ROTATION_PRECISION) + 360.f, 360.f);

	if (enable_rotate)
	{
		mCtrlRotX->set( mCurEulerDegrees.mV[VX] );
		mCtrlRotY->set( mCurEulerDegrees.mV[VY] );
		mCtrlRotZ->set( mCurEulerDegrees.mV[VZ] );
		calcp->setVar(LLCalc::X_ROT, mCurEulerDegrees.mV[VX]);
		calcp->setVar(LLCalc::Y_ROT, mCurEulerDegrees.mV[VY]);
		calcp->setVar(LLCalc::Z_ROT, mCurEulerDegrees.mV[VZ]);
	}
	else
	{
		mCtrlRotX->clear();
		mCtrlRotY->clear();
		mCtrlRotZ->clear();
		calcp->clearVar(LLCalc::X_ROT);
		calcp->clearVar(LLCalc::Y_ROT);
		calcp->clearVar(LLCalc::Z_ROT);
	}

	mLabelRotation->setEnabled( enable_rotate );
	mCtrlRotX->setEnabled( enable_rotate );
	mCtrlRotY->setEnabled( enable_rotate );
	mCtrlRotZ->setEnabled( enable_rotate );

	mBtnCopyPos->setEnabled(enable_move);
	mBtnPastePos->setEnabled(enable_move);
	mBtnPastePosClip->setEnabled(enable_move);
	mBtnCopySize->setEnabled( enable_scale );
	mBtnPasteSize->setEnabled( enable_scale );
	mBtnPasteSizeClip->setEnabled( enable_scale );
	mBtnCopyRot->setEnabled( enable_rotate );
	mBtnPasteRot->setEnabled( enable_rotate );
	mBtnPasteRotClip->setEnabled( enable_rotate );
	mBtnCopyParams->setEnabled( single_volume && objectp->permModify() );
	mBtnPasteParams->setEnabled( single_volume && objectp->permModify() );

	BOOL owners_identical;
	LLUUID owner_id;
	std::string owner_name;
	owners_identical = LLSelectMgr::getInstance()->selectGetOwner(owner_id, owner_name);

	// BUG? Check for all objects being editable?
	S32 roots_selected = LLSelectMgr::getInstance()->getSelection()->getRootObjectCount();
	BOOL editable = root_objectp->permModify();

	// Select Single Message
	getChildView("select_single")->setVisible( FALSE);
	getChildView("edit_object")->setVisible( FALSE);
	if (!editable || single_volume || selected_count <= 1)
	{
		getChildView("edit_object")->setVisible( TRUE);
		getChildView("edit_object")->setEnabled(TRUE);
	}
	else
	{
		getChildView("select_single")->setVisible( TRUE);
		getChildView("select_single")->setEnabled(TRUE);
	}

	BOOL is_flexible = volobjp && volobjp->isFlexible();
	BOOL is_permanent = root_objectp->flagObjectPermanent();
	BOOL is_permanent_enforced = root_objectp->isPermanentEnforced();
	BOOL is_character = root_objectp->flagCharacter();
	llassert(!is_permanent || !is_character); // should never have a permanent object that is also a character

	// Lock checkbox - only modifiable if you own the object.
	BOOL self_owned = (gAgent.getID() == owner_id);
	mCheckLock->setEnabled( roots_selected > 0 && self_owned && !is_permanent_enforced);

	// More lock and debit checkbox - get the values
	BOOL valid;
	U32 owner_mask_on;
	U32 owner_mask_off;
	valid = LLSelectMgr::getInstance()->selectGetPerm(PERM_OWNER, &owner_mask_on, &owner_mask_off);

	if(valid)
	{
		if(owner_mask_on & PERM_MOVE)
		{
			// owner can move, so not locked
			mCheckLock->set(FALSE);
			mCheckLock->setTentative(FALSE);
		}
		else if(owner_mask_off & PERM_MOVE)
		{
			// owner can't move, so locked
			mCheckLock->set(TRUE);
			mCheckLock->setTentative(FALSE);
		}
		else
		{
			// some locked, some not locked
			mCheckLock->set(FALSE);
			mCheckLock->setTentative(TRUE);
		}
	}

	// Physics checkbox
	mIsPhysical = root_objectp->flagUsePhysics();
	llassert(!is_permanent || !mIsPhysical); // should never have a permanent object that is also physical

	mCheckPhysics->set( mIsPhysical );
	mCheckPhysics->setEnabled( roots_selected>0 
								&& (editable || gAgent.isGodlike()) 
								&& !is_flexible && !is_permanent);

	mIsTemporary = root_objectp->flagTemporaryOnRez();
	llassert(!is_permanent || !mIsTemporary); // should never has a permanent object that is also temporary

	mCheckTemporary->set( mIsTemporary );
	mCheckTemporary->setEnabled( roots_selected>0 && editable && !is_permanent);

	mIsPhantom = root_objectp->flagPhantom();
	BOOL is_volume_detect = root_objectp->flagVolumeDetect();
	llassert(!is_character || !mIsPhantom); // should never have a character that is also a phantom
	mCheckPhantom->set( mIsPhantom );
	mCheckPhantom->setEnabled( roots_selected>0 && editable && !is_flexible && !is_permanent_enforced && !is_character && !is_volume_detect);

	//----------------------------------------------------------------------------

	S32 selected_item	= MI_BOX;
	S32	selected_hole	= MI_HOLE_SAME;
	BOOL enabled = FALSE;
	BOOL hole_enabled = FALSE;
	F32 scale_x=1.f, scale_y=1.f;
	BOOL isMesh = FALSE;
	
	if( !objectp || !objectp->getVolume() || !editable || !single_volume)
	{
		// Clear out all geometry fields.
		mComboBaseType->clear();
		mSpinHollow->clear();
		mSpinCutBegin->clear();
		mSpinCutEnd->clear();
		mCtrlPathBegin->clear();
		mCtrlPathEnd->clear();
		mSpinScaleX->clear();
		mSpinScaleY->clear();
		mSpinTwist->clear();
		mSpinTwistBegin->clear();
		mComboHoleType->clear();
		mSpinShearX->clear();
		mSpinShearY->clear();
		mSpinTaperX->clear();
		mSpinTaperY->clear();
		mSpinRadiusOffset->clear();
		mSpinRevolutions->clear();
		mSpinSkew->clear();
		
		mSelectedType = MI_NONE;
	}
	else
	{
		// Only allowed to change these parameters for objects
		// that you have permissions on AND are not attachments.
		enabled = root_objectp->permModify() && !root_objectp->isPermanentEnforced();
		
		// Volume type
		const LLVolumeParams &volume_params = objectp->getVolume()->getParams();
		U8 path = volume_params.getPathParams().getCurveType();
		U8 profile_and_hole = volume_params.getProfileParams().getCurveType();
		U8 profile	= profile_and_hole & LL_PCODE_PROFILE_MASK;
		U8 hole		= profile_and_hole & LL_PCODE_HOLE_MASK;
		
		// Scale goes first so we can differentiate between a sphere and a torus,
		// which have the same profile and path types.

		// Scale
		scale_x = volume_params.getRatioX();
		scale_y = volume_params.getRatioY();

		BOOL linear_path = (path == LL_PCODE_PATH_LINE) || (path == LL_PCODE_PATH_FLEXIBLE);
		if ( linear_path && profile == LL_PCODE_PROFILE_CIRCLE )
		{
			selected_item = MI_CYLINDER;
		}
		else if ( linear_path && profile == LL_PCODE_PROFILE_SQUARE )
		{
			selected_item = MI_BOX;
		}
		else if ( linear_path && profile == LL_PCODE_PROFILE_ISOTRI )
		{
			selected_item = MI_PRISM;
		}
		else if ( linear_path && profile == LL_PCODE_PROFILE_EQUALTRI )
		{
			selected_item = MI_PRISM;
		}
		else if ( linear_path && profile == LL_PCODE_PROFILE_RIGHTTRI )
		{
			selected_item = MI_PRISM;
		}
		else if (path == LL_PCODE_PATH_FLEXIBLE) // shouldn't happen
		{
			selected_item = MI_CYLINDER; // reasonable default
		}
		else if ( path == LL_PCODE_PATH_CIRCLE && profile == LL_PCODE_PROFILE_CIRCLE && scale_y > 0.75f)
		{
			selected_item = MI_SPHERE;
		}
		else if ( path == LL_PCODE_PATH_CIRCLE && profile == LL_PCODE_PROFILE_CIRCLE && scale_y <= 0.75f)
		{
			selected_item = MI_TORUS;
		}
		else if ( path == LL_PCODE_PATH_CIRCLE && profile == LL_PCODE_PROFILE_CIRCLE_HALF)
		{
			selected_item = MI_SPHERE;
		}
		else if ( path == LL_PCODE_PATH_CIRCLE2 && profile == LL_PCODE_PROFILE_CIRCLE )
		{
			// Spirals aren't supported.  Make it into a sphere.  JC
			selected_item = MI_SPHERE;
		}
		else if ( path == LL_PCODE_PATH_CIRCLE && profile == LL_PCODE_PROFILE_EQUALTRI )
		{
			selected_item = MI_RING;
		}
		else if ( path == LL_PCODE_PATH_CIRCLE && profile == LL_PCODE_PROFILE_SQUARE && scale_y <= 0.75f)
		{
			selected_item = MI_TUBE;
		}
		else if ( path == LL_PCODE_PATH_CIRCLE2 && profile == LL_PCODE_PROFILE_EQUALTRI )
		{
			selected_item = MI_PATH_CIRCLE2_PROFILE_TRI;
		}
		else if ( path == LL_PCODE_PATH_CIRCLE2 && profile == LL_PCODE_PROFILE_SQUARE )
		{
			selected_item = MI_PATH_CIRCLE2_PROFILE_SQUARE;
		}
		else if ( path == LL_PCODE_PATH_CIRCLE2 && profile == LL_PCODE_PROFILE_CIRCLE_HALF )
		{
			selected_item = MI_PATH_CIRCLE2_PROFILE_CIRCLE_HALF;
		}
		else if ( path == LL_PCODE_PATH_TEST && profile == LL_PCODE_PROFILE_SQUARE )
		{
			selected_item = MI_PATH_TEST_PROFILE_SQUARE;
		}
		else if ( path == LL_PCODE_PATH_TEST && profile == LL_PCODE_PROFILE_EQUALTRI )
		{
			selected_item = MI_PATH_TEST_PROFILE_TRI;
		}
		else if ( path == LL_PCODE_PATH_TEST && profile == LL_PCODE_PROFILE_CIRCLE )
		{
			selected_item = MI_PATH_TEST_PROFILE_CIRCLE;
		}
		else if ( path == LL_PCODE_PATH_TEST && profile == LL_PCODE_PROFILE_CIRCLE_HALF )
		{
			selected_item = MI_PATH_TEST_PROFILE_CIRCLE_HALF;
		}
		else if ( path == LL_PCODE_PATH_LINE && profile == LL_PCODE_PROFILE_CIRCLE_HALF )
		{
			selected_item = MI_PATH_LINE_PROFILE_CIRCLE_HALF;
		}
		//<-- Working33 by Gregory Maurer
		else if ( path == LL_PCODE_PATH_CIRCLE_33 && profile == LL_PCODE_PROFILE_CIRCLE )
		{
			selected_item = MI_PATH_33_PROFILE_CIRCLE;
		}
		else if ( path == LL_PCODE_PATH_CIRCLE_33 && profile == LL_PCODE_PROFILE_SQUARE )
		{
			selected_item = MI_PATH_33_PROFILE_SQUARE;
		}
		else if ( path == LL_PCODE_PATH_CIRCLE_33 && profile == LL_PCODE_PROFILE_ISOTRI )
		{
			selected_item = MI_PATH_33_PROFILE_TRIANGLE;
		}
		else if ( path == LL_PCODE_PATH_CIRCLE_33 && profile == LL_PCODE_PROFILE_CIRCLE_HALF )
		{
			selected_item = MI_PATH_33_PROFILE_HALFCIRCLE;
		}
		//Working33 -->
		else
		{
			llinfos << "Unknown path " << (S32) path << " profile " << (S32) profile << " in getState" << llendl;
			selected_item = MI_BOX;
		}


		if (objectp->getParameterEntryInUse(LLNetworkData::PARAMS_SCULPT))
		{
			selected_item = MI_SCULPT;
			//LLFirstUse::useSculptedPrim();
		}

		
		mComboBaseType	->setCurrentByIndex( selected_item );
		mSelectedType = selected_item;
		
		// Grab S path
		F32 begin_s	= volume_params.getBeginS();
		F32 end_s	= volume_params.getEndS();

		// Compute cut and advanced cut from S and T
		F32 begin_t = volume_params.getBeginT();
		F32 end_t	= volume_params.getEndT();

		// Hollowness
		F32 hollow = 100.f * volume_params.getHollow();
		mSpinHollow->set( hollow );
		calcp->setVar(LLCalc::HOLLOW, hollow);
		// All hollow objects allow a shape to be selected.
		if (hollow > 0.f)
		{
			switch (hole)
			{
			case LL_PCODE_HOLE_CIRCLE:
				selected_hole = MI_HOLE_CIRCLE;
				break;
			case LL_PCODE_HOLE_SQUARE:
				selected_hole = MI_HOLE_SQUARE;
				break;
			case LL_PCODE_HOLE_TRIANGLE:
				selected_hole = MI_HOLE_TRIANGLE;
				break;
			case LL_PCODE_HOLE_SAME:
			default:
				selected_hole = MI_HOLE_SAME;
				break;
			}
			mComboHoleType->setCurrentByIndex( selected_hole );
			hole_enabled = enabled;
		}
		else
		{
			mComboHoleType->setCurrentByIndex( MI_HOLE_SAME );
			hole_enabled = FALSE;
		}

		// Cut interpretation varies based on base object type
		F32 cut_begin, cut_end, adv_cut_begin, adv_cut_end;

		if ( selected_item == MI_SPHERE || selected_item == MI_TORUS || 
			 selected_item == MI_TUBE   || selected_item == MI_RING )
		{
			cut_begin		= begin_t;
			cut_end			= end_t;
			adv_cut_begin	= begin_s;
			adv_cut_end		= end_s;
		}
		else
		{
			cut_begin       = begin_s;
			cut_end         = end_s;
			adv_cut_begin   = begin_t;
			adv_cut_end     = end_t;
		}

		mSpinCutBegin	->set( cut_begin );
		mSpinCutEnd		->set( cut_end );
		mCtrlPathBegin	->set( adv_cut_begin );
		mCtrlPathEnd	->set( adv_cut_end );
		calcp->setVar(LLCalc::CUT_BEGIN, cut_begin);
		calcp->setVar(LLCalc::CUT_END, cut_end);
		calcp->setVar(LLCalc::PATH_BEGIN, adv_cut_begin);
		calcp->setVar(LLCalc::PATH_END, adv_cut_end);

		// Twist
		F32 twist		= volume_params.getTwist();
		F32 twist_begin = volume_params.getTwistBegin();
		// Check the path type for conversion.
		if (path == LL_PCODE_PATH_LINE || path == LL_PCODE_PATH_FLEXIBLE)
		{
			twist		*= OBJECT_TWIST_LINEAR_MAX;
			twist_begin	*= OBJECT_TWIST_LINEAR_MAX;
		}
		else
		{
			twist		*= OBJECT_TWIST_MAX;
			twist_begin	*= OBJECT_TWIST_MAX;
		}

		mSpinTwist		->set( twist );
		mSpinTwistBegin	->set( twist_begin );
		calcp->setVar(LLCalc::TWIST_END, twist);
		calcp->setVar(LLCalc::TWIST_BEGIN, twist_begin);

		// Shear
		F32 shear_x = volume_params.getShearX();
		F32 shear_y = volume_params.getShearY();
		mSpinShearX->set( shear_x );
		mSpinShearY->set( shear_y );
		calcp->setVar(LLCalc::X_SHEAR, shear_x);
		calcp->setVar(LLCalc::Y_SHEAR, shear_y);

		// Taper
		F32 taper_x	= volume_params.getTaperX();
		F32 taper_y = volume_params.getTaperY();
		mSpinTaperX->set( taper_x );
		mSpinTaperY->set( taper_y );
		calcp->setVar(LLCalc::X_TAPER, taper_x);
		calcp->setVar(LLCalc::Y_TAPER, taper_y);

		// Radius offset.
		F32 radius_offset = volume_params.getRadiusOffset();
		// Limit radius offset, based on taper and hole size y.
//KC: Phoenix capability
#if 0
		F32 radius_mag = fabs(radius_offset);
		F32 hole_y_mag = fabs(scale_y);
		F32 taper_y_mag  = fabs(taper_y);
		// Check to see if the taper effects us.
		if ( (radius_offset > 0.f && taper_y < 0.f) ||
			 (radius_offset < 0.f && taper_y > 0.f) )
		{
			// The taper does not help increase the radius offset range.
			taper_y_mag = 0.f;
		}
		F32 max_radius_mag = 1.f - hole_y_mag * (1.f - taper_y_mag) / (1.f - hole_y_mag);
		// Enforce the maximum magnitude.
		if (radius_mag > max_radius_mag)
		{
			// Check radius offset sign.
			if (radius_offset < 0.f)
			{
				radius_offset = -max_radius_mag;
			}
			else
			{
				radius_offset = max_radius_mag;
			}
		}
#endif
		mSpinRadiusOffset->set( radius_offset);
		calcp->setVar(LLCalc::RADIUS_OFFSET, radius_offset);

		// Revolutions
		F32 revolutions = volume_params.getRevolutions();
		mSpinRevolutions->set( revolutions );
		calcp->setVar(LLCalc::REVOLUTIONS, revolutions);
		
		// Skew
		F32 skew	= volume_params.getSkew();
		// Limit skew, based on revolutions hole size x.
//KC: Phoenix capability
#if 0
		F32 skew_mag= fabs(skew);
		F32 min_skew_mag = 1.0f - 1.0f / (revolutions * scale_x + 1.0f);
		// Discontinuity; A revolution of 1 allows skews below 0.5.
		if ( fabs(revolutions - 1.0f) < 0.001)
			min_skew_mag = 0.0f;

		// Clip skew.
		if (skew_mag < min_skew_mag)
		{
			// Check skew sign.
			if (skew < 0.0f)
			{
				skew = -min_skew_mag;
			}
			else 
			{
				skew = min_skew_mag;
			}
		}
#endif
		mSpinSkew->set( skew );
		calcp->setVar(LLCalc::SKEW, skew);
	}
	
	// Compute control visibility, label names, and twist range.
	// Start with defaults.
	BOOL cut_visible                = TRUE;
	BOOL hollow_visible             = TRUE;
	BOOL top_size_x_visible			= TRUE;
	BOOL top_size_y_visible			= TRUE;
	BOOL top_shear_x_visible		= TRUE;
	BOOL top_shear_y_visible		= TRUE;
	BOOL twist_visible				= TRUE;
	//KC: Phoenix capability, allow all transforms
	BOOL advanced_cut_visible		= TRUE;
	BOOL taper_visible				= TRUE;
	BOOL skew_visible				= TRUE;
	BOOL radius_offset_visible		= TRUE;
	BOOL revolutions_visible		= TRUE;
	BOOL sculpt_texture_visible     = FALSE;
	F32	 twist_min					= OBJECT_TWIST_LINEAR_MIN;
	F32	 twist_max					= OBJECT_TWIST_LINEAR_MAX;
	F32	 twist_inc					= OBJECT_TWIST_LINEAR_INC;

	BOOL advanced_is_dimple = FALSE;
	BOOL advanced_is_slice = FALSE;
	BOOL size_is_hole = FALSE;

	// Tune based on overall volume type
	switch (selected_item)
	{
	case MI_SPHERE:
		top_size_x_visible		= TRUE;
		top_size_y_visible		= TRUE;
		top_shear_x_visible		= TRUE;
		top_shear_y_visible		= TRUE;
		twist_visible			= TRUE;
		advanced_cut_visible	= TRUE;
		advanced_is_dimple		= TRUE;
		twist_min				= OBJECT_TWIST_MIN;
		twist_max				= OBJECT_TWIST_MAX;
		twist_inc				= OBJECT_TWIST_INC;
		break;

	case MI_TORUS:
	case MI_TUBE:	
	case MI_RING:
		//top_size_x_visible		= FALSE;
		//top_size_y_visible		= FALSE;
	  	size_is_hole 			= TRUE;
		skew_visible			= TRUE;
		advanced_cut_visible	= TRUE;
		taper_visible			= TRUE;
		radius_offset_visible	= TRUE;
		revolutions_visible		= TRUE;
		twist_min				= OBJECT_TWIST_MIN;
		twist_max				= OBJECT_TWIST_MAX;
		twist_inc				= OBJECT_TWIST_INC;

		break;

	case MI_SCULPT:
		cut_visible             = FALSE;
		hollow_visible          = FALSE;
		twist_visible           = FALSE;
		top_size_x_visible      = FALSE;
		top_size_y_visible      = FALSE;
		top_shear_x_visible     = FALSE;
		top_shear_y_visible     = FALSE;
		skew_visible            = FALSE;
		advanced_cut_visible    = FALSE;
		taper_visible           = FALSE;
		radius_offset_visible   = FALSE;
		revolutions_visible     = FALSE;
		sculpt_texture_visible  = TRUE;

		break;
		
	case MI_BOX:
		advanced_cut_visible	= TRUE;
		advanced_is_slice		= TRUE;
		taper_visible			= FALSE; //KC: does nothing for boxes
		break;

	case MI_CYLINDER:
		advanced_cut_visible	= TRUE;
		advanced_is_slice		= TRUE;
		break;

	case MI_PRISM:
		advanced_cut_visible	= TRUE;
		advanced_is_slice		= TRUE;
		break;

	default:
		break;
	}

	// Check if we need to change top size/hole size params.
	switch (selected_item)
	{
	case MI_SPHERE:
		mSpinScaleX->set( scale_x );
		mSpinScaleY->set( scale_y );
		calcp->setVar(LLCalc::X_HOLE, scale_x);
		calcp->setVar(LLCalc::Y_HOLE, scale_y);
		mSpinScaleX->setMinValue(0.0f);
		mSpinScaleX->setMaxValue(1.0f);
		mSpinScaleY->setMinValue(0.0f);
		mSpinScaleY->setMaxValue(1.0f);
		break;
	case MI_TORUS:
	case MI_TUBE:
	case MI_RING:
		mSpinScaleX->set( scale_x );
		mSpinScaleY->set( scale_y );
		mSpinScaleX->setMinValue(mMinHoleSize);// <AW: opensim-limits>
		calcp->setVar(LLCalc::X_HOLE, scale_x);
		calcp->setVar(LLCalc::Y_HOLE, scale_y);
//		mSpinScaleX->setMinValue(OBJECT_MIN_HOLE_SIZE);
		mSpinScaleX->setMinValue(mMinHoleSize);// <AW: opensim-limits>
		mSpinScaleX->setMaxValue(OBJECT_MAX_HOLE_SIZE_X);
//		mSpinScaleY->setMinValue(OBJECT_MIN_HOLE_SIZE);
		mSpinScaleY->setMinValue(mMinHoleSize);// <AW: opensim-limits>
		mSpinScaleY->setMaxValue(OBJECT_MAX_HOLE_SIZE_Y);
		break;
	default:
		if (editable)
		{
			F32 tmp_scale_x = scale_x;
			F32 tmp_scale_y = scale_y;
			if ( selected_item == MI_BOX || selected_item == MI_CYLINDER || selected_item == MI_PRISM)
			{
				tmp_scale_x = 1.f - tmp_scale_x;
				tmp_scale_y = 1.f - tmp_scale_y;
			}
			mSpinScaleX->set( tmp_scale_x );
			mSpinScaleY->set( tmp_scale_y );
			mSpinScaleX->setMinValue(-4000.f);
			mSpinScaleX->setMaxValue(4000.f);
			mSpinScaleY->setMinValue(-4000.f);
			mSpinScaleY->setMaxValue(4000.f);

			// Torus' Hole Size is Box/Cyl/Prism's Taper
			calcp->setVar(LLCalc::X_TAPER, 1.f - scale_x);
			calcp->setVar(LLCalc::Y_TAPER, 1.f - scale_y);

			// Box/Cyl/Prism have no hole size
			calcp->setVar(LLCalc::X_HOLE, 0.f);
			calcp->setVar(LLCalc::Y_HOLE, 0.f);
		}
		break;
	}

	// Check if we need to limit the hollow based on the hole type.
//KC: Phoenix capability
#if 0
	if (  selected_hole == MI_HOLE_SQUARE && 
		  ( selected_item == MI_CYLINDER || selected_item == MI_TORUS ||
		    selected_item == MI_PRISM    || selected_item == MI_RING  ||
			selected_item == MI_SPHERE ) )
	{
		mSpinHollow->setMinValue(0.f);
		mSpinHollow->setMaxValue(70.f);
	}
	else 
#endif
	{
		mSpinHollow->setMinValue(0.f);
// 		mSpinHollow->setMaxValue(100.f);
		mSpinHollow->setMaxValue(mMaxHollowSize);// <AW: opensim-limits>
	}

	// Update field enablement
	mComboBaseType	->setEnabled( enabled );

	mLabelCut		->setEnabled( enabled );
	mSpinCutBegin	->setEnabled( enabled );
	mSpinCutEnd		->setEnabled( enabled );

	mLabelHollow	->setEnabled( enabled );
	mSpinHollow		->setEnabled( enabled );
	mLabelHoleType	->setEnabled( hole_enabled );
	mComboHoleType	->setEnabled( hole_enabled );

	mLabelTwist		->setEnabled( enabled );
	mSpinTwist		->setEnabled( enabled );
	mSpinTwistBegin	->setEnabled( enabled );

	mLabelSkew		->setEnabled( enabled );
	mSpinSkew		->setEnabled( enabled );

	getChildView("scale_hole")->setVisible( FALSE);
	getChildView("scale_taper")->setVisible( FALSE);
	if (top_size_x_visible || top_size_y_visible)
	{
		if (size_is_hole)
		{
			getChildView("scale_hole")->setVisible( TRUE);
			getChildView("scale_hole")->setEnabled(enabled);
		}
		else
		{
			getChildView("scale_taper")->setVisible( TRUE);
			getChildView("scale_taper")->setEnabled(enabled);
		}
	}
	
	mSpinScaleX		->setEnabled( enabled );
	mSpinScaleY		->setEnabled( enabled );

	mLabelShear		->setEnabled( enabled );
	mSpinShearX		->setEnabled( enabled );
	mSpinShearY		->setEnabled( enabled );

	getChildView("advanced_cut")->setVisible( FALSE);
	getChildView("advanced_dimple")->setVisible( FALSE);
	getChildView("advanced_slice")->setVisible( FALSE);

	if (advanced_cut_visible)
	{
		if (advanced_is_dimple)
		{
			getChildView("advanced_dimple")->setVisible( TRUE);
			getChildView("advanced_dimple")->setEnabled(enabled);
		}

		else if (advanced_is_slice)
		{
			getChildView("advanced_slice")->setVisible( TRUE);
			getChildView("advanced_slice")->setEnabled(enabled);
		}
		else
		{
			getChildView("advanced_cut")->setVisible( TRUE);
			getChildView("advanced_cut")->setEnabled(enabled);
		}
	}
	
	mCtrlPathBegin	->setEnabled( enabled );
	mCtrlPathEnd	->setEnabled( enabled );

	mLabelTaper		->setEnabled( enabled );
	mSpinTaperX		->setEnabled( enabled );
	mSpinTaperY		->setEnabled( enabled );

	mLabelRadiusOffset->setEnabled( enabled );
	mSpinRadiusOffset ->setEnabled( enabled );

	mLabelRevolutions->setEnabled( enabled );
	mSpinRevolutions ->setEnabled( enabled );

	// Update field visibility
	mLabelCut		->setVisible( cut_visible );
	mSpinCutBegin	->setVisible( cut_visible );
	mSpinCutEnd		->setVisible( cut_visible ); 

	mLabelHollow	->setVisible( hollow_visible );
	mSpinHollow		->setVisible( hollow_visible );
	mLabelHoleType	->setVisible( hollow_visible );
	mComboHoleType	->setVisible( hollow_visible );
	
	mLabelTwist		->setVisible( twist_visible );
	mSpinTwist		->setVisible( twist_visible );
	mSpinTwistBegin	->setVisible( twist_visible );
	mSpinTwist		->setMinValue(  twist_min );
	mSpinTwist		->setMaxValue(  twist_max );
	mSpinTwist		->setIncrement( twist_inc );
	mSpinTwistBegin	->setMinValue(  twist_min );
	mSpinTwistBegin	->setMaxValue(  twist_max );
	mSpinTwistBegin	->setIncrement( twist_inc );

	mSpinScaleX		->setVisible( top_size_x_visible );
	mSpinScaleY		->setVisible( top_size_y_visible );

	mLabelSkew		->setVisible( skew_visible );
	mSpinSkew		->setVisible( skew_visible );

	mLabelShear		->setVisible( top_shear_x_visible || top_shear_y_visible );
	mSpinShearX		->setVisible( top_shear_x_visible );
	mSpinShearY		->setVisible( top_shear_y_visible );

	mCtrlPathBegin	->setVisible( advanced_cut_visible );
	mCtrlPathEnd	->setVisible( advanced_cut_visible );

	mLabelTaper		->setVisible( taper_visible );
	mSpinTaperX		->setVisible( taper_visible );
	mSpinTaperY		->setVisible( taper_visible );

	mLabelRadiusOffset->setVisible( radius_offset_visible );
	mSpinRadiusOffset ->setVisible( radius_offset_visible );

	mLabelRevolutions->setVisible( revolutions_visible );
	mSpinRevolutions ->setVisible( revolutions_visible );

	mCtrlSculptTexture->setVisible(sculpt_texture_visible);
	mLabelSculptType->setVisible(sculpt_texture_visible);
	mCtrlSculptType->setVisible(sculpt_texture_visible);


	// sculpt texture
	if (selected_item == MI_SCULPT)
	{


		LLUUID id;
		LLSculptParams *sculpt_params = (LLSculptParams *)objectp->getParameterEntry(LLNetworkData::PARAMS_SCULPT);

		
		if (sculpt_params) // if we have a legal sculpt param block for this object:
		{
			if (mObject != objectp)  // we've just selected a new object, so save for undo
			{
				mSculptTextureRevert = sculpt_params->getSculptTexture();
				mSculptTypeRevert    = sculpt_params->getSculptType();
			}
		
			U8 sculpt_type = sculpt_params->getSculptType();
			U8 sculpt_stitching = sculpt_type & LL_SCULPT_TYPE_MASK;
			BOOL sculpt_invert = sculpt_type & LL_SCULPT_FLAG_INVERT;
			BOOL sculpt_mirror = sculpt_type & LL_SCULPT_FLAG_MIRROR;
			isMesh = (sculpt_stitching == LL_SCULPT_TYPE_MESH);

			LLTextureCtrl*  mTextureCtrl = getChild<LLTextureCtrl>("sculpt texture control");
			if(mTextureCtrl)
			{
				mTextureCtrl->setTentative(FALSE);
				mTextureCtrl->setEnabled(editable && !isMesh);
				if (editable)
					mTextureCtrl->setImageAssetID(sculpt_params->getSculptTexture());
				else
					mTextureCtrl->setImageAssetID(LLUUID::null);
			}

			mComboBaseType->setEnabled(!isMesh);
			
			if (mCtrlSculptType)
			{
				if (sculpt_stitching == LL_SCULPT_TYPE_NONE)
				{
					// since 'None' is no longer an option in the combo box
					// use 'Plane' as an equivalent sculpt type
					mCtrlSculptType->setSelectedByValue(LLSD(LL_SCULPT_TYPE_PLANE), true);
				}
				else
				{
					mCtrlSculptType->setSelectedByValue(LLSD(sculpt_stitching), true);
				}
				mCtrlSculptType->setEnabled(editable && !isMesh);
			}

			if (mCtrlSculptMirror)
			{
				mCtrlSculptMirror->set(sculpt_mirror);
				mCtrlSculptMirror->setEnabled(editable && !isMesh);
			}

			if (mCtrlSculptInvert)
			{
				mCtrlSculptInvert->set(sculpt_invert);
				mCtrlSculptInvert->setEnabled(editable);
			}

			if (mLabelSculptType)
			{
				mLabelSculptType->setEnabled(TRUE);
			}
			
		}
	}
	else
	{
		mSculptTextureRevert = LLUUID::null;		
	}

	mCtrlSculptMirror->setVisible(sculpt_texture_visible && !isMesh);
	mCtrlSculptInvert->setVisible(sculpt_texture_visible && !isMesh);

	//----------------------------------------------------------------------------

	mObject = objectp;
	mRootObject = root_objectp;
}

// static
bool LLPanelObject::precommitValidate( const LLSD& data )
{
	// TODO: Richard will fill this in later.  
	return TRUE; // FALSE means that validation failed and new value should not be commited.
}

void LLPanelObject::sendIsPhysical()
{
	BOOL value = mCheckPhysics->get();
	if( mIsPhysical != value )
	{
		LLSelectMgr::getInstance()->selectionUpdatePhysics(value);
		mIsPhysical = value;

		llinfos << "update physics sent" << llendl;
	}
	else
	{
		llinfos << "update physics not changed" << llendl;
	}
}

void LLPanelObject::sendIsTemporary()
{
	BOOL value = mCheckTemporary->get();
	if( mIsTemporary != value )
	{
		LLSelectMgr::getInstance()->selectionUpdateTemporary(value);
		mIsTemporary = value;

		llinfos << "update temporary sent" << llendl;
	}
	else
	{
		llinfos << "update temporary not changed" << llendl;
	}
}


void LLPanelObject::sendIsPhantom()
{
	BOOL value = mCheckPhantom->get();
	if( mIsPhantom != value )
	{
		LLSelectMgr::getInstance()->selectionUpdatePhantom(value);
		mIsPhantom = value;

		llinfos << "update phantom sent" << llendl;
	}
	else
	{
		llinfos << "update phantom not changed" << llendl;
	}
}

// static
void LLPanelObject::onCommitParametric( LLUICtrl* ctrl, void* userdata )
{
	LLPanelObject* self = (LLPanelObject*) userdata;

	if (self->mObject.isNull())
	{
		return;
	}

	if (self->mObject->getPCode() != LL_PCODE_VOLUME)
	{
		// Don't allow modification of non-volume objects.
		return;
	}

	LLVolume *volume = self->mObject->getVolume();
	if (!volume)
	{
		return;
	}

	LLVolumeParams volume_params;
	self->getVolumeParams(volume_params);
	

	
	// set sculpting
	S32 selected_type = self->mComboBaseType->getCurrentIndex();

	if (selected_type == MI_SCULPT)
	{
		self->mObject->setParameterEntryInUse(LLNetworkData::PARAMS_SCULPT, TRUE, TRUE);
		LLSculptParams *sculpt_params = (LLSculptParams *)self->mObject->getParameterEntry(LLNetworkData::PARAMS_SCULPT);
		if (sculpt_params)
			volume_params.setSculptID(sculpt_params->getSculptTexture(), sculpt_params->getSculptType());
	}
	else
	{
		LLSculptParams *sculpt_params = (LLSculptParams *)self->mObject->getParameterEntry(LLNetworkData::PARAMS_SCULPT);
		if (sculpt_params)
			self->mObject->setParameterEntryInUse(LLNetworkData::PARAMS_SCULPT, FALSE, TRUE);
	}

	// Update the volume, if necessary.
	self->mObject->updateVolume(volume_params);


	// This was added to make sure thate when changes are made, the UI
	// adjusts to present valid options.
	// *FIX: only some changes, ie, hollow or primitive type changes,
	// require a refresh.
	self->refresh();

}

void LLPanelObject::getVolumeParams(LLVolumeParams& volume_params)
{
	// Figure out what type of volume to make
	S32 was_selected_type = mSelectedType;
	S32 selected_type = mComboBaseType->getCurrentIndex();
	U8 profile;
	U8 path;
	switch ( selected_type )
	{
	case MI_CYLINDER:
		profile = LL_PCODE_PROFILE_CIRCLE;
		path = LL_PCODE_PATH_LINE;
		break;

	case MI_BOX:
		profile = LL_PCODE_PROFILE_SQUARE;
		path = LL_PCODE_PATH_LINE;
		break;

	case MI_PRISM:
		profile = LL_PCODE_PROFILE_EQUALTRI;
		path = LL_PCODE_PATH_LINE;
		break;

	case MI_SPHERE:
		profile = LL_PCODE_PROFILE_CIRCLE_HALF;
		path = LL_PCODE_PATH_CIRCLE;
		break;

	case MI_TORUS:
		profile = LL_PCODE_PROFILE_CIRCLE;
		path = LL_PCODE_PATH_CIRCLE;
		break;

	case MI_TUBE:
		profile = LL_PCODE_PROFILE_SQUARE;
		path = LL_PCODE_PATH_CIRCLE;
		break;

	case MI_RING:
		profile = LL_PCODE_PROFILE_EQUALTRI;
		path = LL_PCODE_PATH_CIRCLE;
		break;

	case MI_SCULPT:
		profile = LL_PCODE_PROFILE_CIRCLE;
		path = LL_PCODE_PATH_CIRCLE;
		break;
		
	case MI_PATH_LINE_PROFILE_CIRCLE_HALF:
		profile = LL_PCODE_PROFILE_CIRCLE_HALF;
		path = LL_PCODE_PATH_LINE;
		break;

	case MI_PATH_CIRCLE_PROFILE_CIRCLE_HALF:
		profile = LL_PCODE_PROFILE_CIRCLE_HALF;
		path = LL_PCODE_PATH_CIRCLE;
		break;

	case MI_PATH_CIRCLE2_PROFILE_SQUARE:
		profile = LL_PCODE_PROFILE_SQUARE;
		path = LL_PCODE_PATH_CIRCLE2;
		break;

	case MI_PATH_CIRCLE2_PROFILE_TRI:
		profile = LL_PCODE_PROFILE_EQUALTRI;
		path = LL_PCODE_PATH_CIRCLE2;
		break;

	case MI_PATH_CIRCLE2_PROFILE_CIRCLE:
		profile = LL_PCODE_PROFILE_CIRCLE;
		path = LL_PCODE_PATH_CIRCLE2;
		break;

	case MI_PATH_CIRCLE2_PROFILE_CIRCLE_HALF:
		profile = LL_PCODE_PROFILE_CIRCLE_HALF;
		path = LL_PCODE_PATH_CIRCLE2;
		break;

	case MI_PATH_TEST_PROFILE_SQUARE:
		profile = LL_PCODE_PROFILE_SQUARE;
		path = LL_PCODE_PATH_TEST;
		break;

	case MI_PATH_TEST_PROFILE_TRI:
		profile = LL_PCODE_PROFILE_EQUALTRI;
		path = LL_PCODE_PATH_TEST;
		break;

	case MI_PATH_TEST_PROFILE_CIRCLE:
		profile = LL_PCODE_PROFILE_CIRCLE;
		path = LL_PCODE_PATH_TEST;
		break;

	case MI_PATH_TEST_PROFILE_CIRCLE_HALF:
		profile = LL_PCODE_PROFILE_CIRCLE_HALF;
		path = LL_PCODE_PATH_TEST;
		break;

//<-- Working33 by Gregory Maurer
	case MI_PATH_33_PROFILE_CIRCLE:
		profile = LL_PCODE_PROFILE_CIRCLE;
		path = LL_PCODE_PATH_CIRCLE_33;
		break;

	case MI_PATH_33_PROFILE_SQUARE:
		profile = LL_PCODE_PROFILE_SQUARE;
		path = LL_PCODE_PATH_CIRCLE_33;
		break;

	case MI_PATH_33_PROFILE_TRIANGLE:
		profile = LL_PCODE_PROFILE_ISOTRI;
		path = LL_PCODE_PATH_CIRCLE_33;
		break;

	case MI_PATH_33_PROFILE_HALFCIRCLE:
		profile = LL_PCODE_PROFILE_CIRCLE_HALF;
		path = LL_PCODE_PATH_CIRCLE_33;
		break;
//Working33 -->

	default:
		llwarns << "Unknown base type " << selected_type 
			<< " in getVolumeParams()" << llendl;
		// assume a box
		selected_type = MI_BOX;
		profile = LL_PCODE_PROFILE_SQUARE;
		path = LL_PCODE_PATH_LINE;
		break;
	}


	if (path == LL_PCODE_PATH_LINE)
	{
		LLVOVolume *volobjp = (LLVOVolume *)(LLViewerObject*)(mObject);
		if (volobjp->isFlexible())
		{
			path = LL_PCODE_PATH_FLEXIBLE;
		}
	}
	
	S32 selected_hole = mComboHoleType->getCurrentIndex();
	U8 hole;
	switch (selected_hole)
	{
	case MI_HOLE_CIRCLE: 
		hole = LL_PCODE_HOLE_CIRCLE;
		break;
	case MI_HOLE_SQUARE:
		hole = LL_PCODE_HOLE_SQUARE;
		break;
	case MI_HOLE_TRIANGLE:
		hole = LL_PCODE_HOLE_TRIANGLE;
		break;
	case MI_HOLE_SAME:
	default:
		hole = LL_PCODE_HOLE_SAME;
		break;
	}

	volume_params.setType(profile | hole, path);
	mSelectedType = selected_type;
	
	// Compute cut start/end
	F32 cut_begin	= mSpinCutBegin->get();
	F32 cut_end		= mSpinCutEnd->get();

	// Make sure at least OBJECT_CUT_INC of the object survives
	if (cut_begin > cut_end - OBJECT_MIN_CUT_INC)
	{
		cut_begin = cut_end - OBJECT_MIN_CUT_INC;
		mSpinCutBegin->set(cut_begin);
	}

	F32 adv_cut_begin	= mCtrlPathBegin->get();
	F32 adv_cut_end		= mCtrlPathEnd->get();

	// Make sure at least OBJECT_CUT_INC of the object survives
	if (adv_cut_begin > adv_cut_end - OBJECT_MIN_CUT_INC)
	{
		adv_cut_begin = adv_cut_end - OBJECT_MIN_CUT_INC;
		mCtrlPathBegin->set(adv_cut_begin);
	}

	F32 begin_s, end_s;
	F32 begin_t, end_t;

	if (selected_type == MI_SPHERE || selected_type == MI_TORUS || 
		selected_type == MI_TUBE   || selected_type == MI_RING)
	{
		begin_s = adv_cut_begin;
		end_s	= adv_cut_end;

		begin_t = cut_begin;
		end_t	= cut_end;
	}
	else
	{
		begin_s = cut_begin;
		end_s	= cut_end;

		begin_t = adv_cut_begin;
		end_t	= adv_cut_end;
	}

	volume_params.setBeginAndEndS(begin_s, end_s);
	volume_params.setBeginAndEndT(begin_t, end_t);

	// Hollowness
	F32 hollow = mSpinHollow->get() / 100.f;

//KC: Phoenix capability
#if 0
	if (  selected_hole == MI_HOLE_SQUARE && 
		( selected_type == MI_CYLINDER || selected_type == MI_TORUS ||
		  selected_type == MI_PRISM    || selected_type == MI_RING  ||
		  selected_type == MI_SPHERE ) )
	{
		if (hollow > 0.7f) hollow = 0.7f;
	}
#endif

	volume_params.setHollow( hollow );

	// Twist Begin,End
	F32 twist_begin = mSpinTwistBegin->get();
	F32 twist		= mSpinTwist->get();
	// Check the path type for twist conversion.
	if (path == LL_PCODE_PATH_LINE || path == LL_PCODE_PATH_FLEXIBLE)
	{
		twist_begin	/= OBJECT_TWIST_LINEAR_MAX;
		twist		/= OBJECT_TWIST_LINEAR_MAX;
	}
	else
	{
		twist_begin	/= OBJECT_TWIST_MAX;
		twist		/= OBJECT_TWIST_MAX;
	}

	volume_params.setTwistBegin(twist_begin);
	volume_params.setTwist(twist);

	// Scale X,Y
	F32 scale_x = mSpinScaleX->get();
	F32 scale_y = mSpinScaleY->get();
	if ( was_selected_type == MI_BOX || was_selected_type == MI_CYLINDER || was_selected_type == MI_PRISM)
	{
		scale_x = 1.f - scale_x;
		scale_y = 1.f - scale_y;
	}
	
	// Skew
	F32 skew = mSpinSkew->get();

	// Taper X,Y
	F32 taper_x = mSpinTaperX->get();
	F32 taper_y = mSpinTaperY->get();

	// Radius offset
	F32 radius_offset = mSpinRadiusOffset->get();

	// Revolutions
	F32 revolutions	  = mSpinRevolutions->get();

//KC: Phoenix capability
#if 0
	if ( selected_type == MI_SPHERE )
	{
		// Snap values to valid sphere parameters.
		scale_x			= 1.0f;
		scale_y			= 1.0f;
		skew			= 0.0f;
		taper_x			= 0.0f;
		taper_y			= 0.0f;
		radius_offset	= 0.0f;
		revolutions		= 1.0f;
	}
	else if ( selected_type == MI_TORUS || selected_type == MI_TUBE ||
			  selected_type == MI_RING )
	{
		scale_x = llclamp(
			scale_x,
//			OBJECT_MIN_HOLE_SIZE,
			mMinHoleSize,// <AW: opensim-limits>
			OBJECT_MAX_HOLE_SIZE_X);
		scale_y = llclamp(
			scale_y,
// 			OBJECT_MIN_HOLE_SIZE,
			mMinHoleSize,// <AW: opensim-limits>
			OBJECT_MAX_HOLE_SIZE_Y);

		// Limit radius offset, based on taper and hole size y.
		F32 radius_mag = fabs(radius_offset);
		F32 hole_y_mag = fabs(scale_y);
		F32 taper_y_mag  = fabs(taper_y);
		// Check to see if the taper effects us.
		if ( (radius_offset > 0.f && taper_y < 0.f) ||
			 (radius_offset < 0.f && taper_y > 0.f) )
		{
			// The taper does not help increase the radius offset range.
			taper_y_mag = 0.f;
		}
		F32 max_radius_mag = 1.f - hole_y_mag * (1.f - taper_y_mag) / (1.f - hole_y_mag);
		// Enforce the maximum magnitude.
		if (radius_mag > max_radius_mag)
		{
			// Check radius offset sign.
			if (radius_offset < 0.f)
			{
				radius_offset = -max_radius_mag;
			}
			else
			{
				radius_offset = max_radius_mag;
			}
		}
			
		// Check the skew value against the revolutions.
		F32 skew_mag= fabs(skew);
		F32 min_skew_mag = 1.0f - 1.0f / (revolutions * scale_x + 1.0f);
		// Discontinuity; A revolution of 1 allows skews below 0.5.
		if ( fabs(revolutions - 1.0f) < 0.001)
			min_skew_mag = 0.0f;

		// Clip skew.
		if (skew_mag < min_skew_mag)
		{
			// Check skew sign.
			if (skew < 0.0f)
			{
				skew = -min_skew_mag;
			}
			else 
			{
				skew = min_skew_mag;
			}
		}
	}
#endif

	volume_params.setRatio( scale_x, scale_y );
	volume_params.setSkew(skew);
	volume_params.setTaper( taper_x, taper_y );
	volume_params.setRadiusOffset(radius_offset);
	volume_params.setRevolutions(revolutions);

	// Shear X,Y
	F32 shear_x = mSpinShearX->get();
	F32 shear_y = mSpinShearY->get();
	volume_params.setShear( shear_x, shear_y );

	if (selected_type == MI_SCULPT)
	{
		volume_params.setSculptID(LLUUID::null, 0);
		volume_params.setBeginAndEndT   (0, 1);
		volume_params.setBeginAndEndS   (0, 1);
		volume_params.setHollow         (0);
		volume_params.setTwistBegin     (0);
		volume_params.setTwistEnd       (0);
		volume_params.setRatio          (1, 0.5);
		volume_params.setShear          (0, 0);
		volume_params.setTaper          (0, 0);
		volume_params.setRevolutions    (1);
		volume_params.setRadiusOffset   (0);
		volume_params.setSkew           (0);
	}

}

// BUG: Make work with multiple objects
void LLPanelObject::sendRotation(BOOL btn_down)
{
	if (mObject.isNull()) return;

	LLVector3 new_rot(mCtrlRotX->get(), mCtrlRotY->get(), mCtrlRotZ->get());
	new_rot.mV[VX] = llround(new_rot.mV[VX], OBJECT_ROTATION_PRECISION);
	new_rot.mV[VY] = llround(new_rot.mV[VY], OBJECT_ROTATION_PRECISION);
	new_rot.mV[VZ] = llround(new_rot.mV[VZ], OBJECT_ROTATION_PRECISION);

	// Note: must compare before conversion to radians
	LLVector3 delta = new_rot - mCurEulerDegrees;

	if (delta.magVec() >= 0.0005f)
	{
		mCurEulerDegrees = new_rot;
		new_rot *= DEG_TO_RAD;

		LLQuaternion rotation;
		rotation.setQuat(new_rot.mV[VX], new_rot.mV[VY], new_rot.mV[VZ]);

		if (mRootObject != mObject)
		{
			// ## Zi: Building spin controls for attachments
			if(mObject->isAttachment())
				rotation = rotation * ~mRootObject->getRotation();
			else
				rotation = rotation * ~mRootObject->getRotationRegion();
			// ## Zi: Building spin controls for attachments
		}
		std::vector<LLVector3>& child_positions = mObject->mUnselectedChildrenPositions ;
		std::vector<LLQuaternion> child_rotations;
		if (mObject->isRootEdit())
		{
			mObject->saveUnselectedChildrenRotation(child_rotations) ;
			mObject->saveUnselectedChildrenPosition(child_positions) ;			
		}

		mObject->setRotation(rotation);
		LLManip::rebuild(mObject) ;

		// for individually selected roots, we need to counterrotate all the children
		if (mObject->isRootEdit())
		{			
			mObject->resetChildrenRotationAndPosition(child_rotations, child_positions) ;			
		}

		if(!btn_down)
		{
			child_positions.clear() ;
			LLSelectMgr::getInstance()->sendMultipleUpdate(UPD_ROTATION | UPD_POSITION);
		}
	}
}

// patch up for mesh, to be removed during real mesh merge -Zi
F32 llpanelobject_max_prim_scale()
{
// <AW: opensim-limits>
//	if(gAgent.getRegion()->getCapability("GetMesh").empty())
//		return DEFAULT_MAX_PRIM_SCALE;
//	return 64.f;
	return LLWorld::getInstance()->getRegionMaxPrimScale();
// </AW: opensim-limits>
}

// BUG: Make work with multiple objects
void LLPanelObject::sendScale(BOOL btn_down)
{
	if (mObject.isNull()) return;

	// make sure not to send larger/smaller values than permitted -Zi
// <AW: opensim-limits>
//	LLVector3 newscale(llclamp(mCtrlScaleX->get(), MIN_PRIM_SCALE, llpanelobject_max_prim_scale()),
//					   llclamp(mCtrlScaleY->get(), MIN_PRIM_SCALE, llpanelobject_max_prim_scale()),
//					   llclamp(mCtrlScaleZ->get(), MIN_PRIM_SCALE, llpanelobject_max_prim_scale()));
	LLVector3 newscale(llclamp(mCtrlScaleX->get(), mMinScale, llpanelobject_max_prim_scale()),
					   llclamp(mCtrlScaleY->get(), mMinScale, llpanelobject_max_prim_scale()),
					   llclamp(mCtrlScaleZ->get(), mMinScale, llpanelobject_max_prim_scale()));
// </AW: opensim-limits>

	LLVector3 delta = newscale - mObject->getScale();
	if (delta.magVec() >= 0.00005f)
	{
		// scale changed by more than 1/20 millimeter

		// check to see if we aren't scaling the textures
		// (in which case the tex coord's need to be recomputed)
		BOOL dont_stretch_textures = !LLManipScale::getStretchTextures();
		if (dont_stretch_textures)
		{
			LLSelectMgr::getInstance()->saveSelectedObjectTransform(SELECT_ACTION_TYPE_SCALE);
		}

		mObject->setScale(newscale, TRUE);

		if(!btn_down)
		{
			LLSelectMgr::getInstance()->sendMultipleUpdate(UPD_SCALE | UPD_POSITION);
		}

		LLSelectMgr::getInstance()->adjustTexturesByScale(TRUE, !dont_stretch_textures);
//		llinfos << "scale sent" << llendl;
	}
	else
	{
//		llinfos << "scale not changed" << llendl;
	}
}


void LLPanelObject::sendPosition(BOOL btn_down)
{	
	if (mObject.isNull()) return;

	LLVector3 newpos(mCtrlPosX->get(), mCtrlPosY->get(), mCtrlPosZ->get());

	LLViewerRegion* regionp = mObject->getRegion();

	// ## Zi: Building spin controls for attachments
	LLVector3d new_pos_global;

	if (mObject->isAttachment())
	{
		newpos.clamp(LLVector3(-MAX_ATTACHMENT_DIST,-MAX_ATTACHMENT_DIST,-MAX_ATTACHMENT_DIST),LLVector3(MAX_ATTACHMENT_DIST,MAX_ATTACHMENT_DIST,MAX_ATTACHMENT_DIST));
	}
	// ## Zi: Building spin controls for attachments
	else
	{
		// Clamp the Z height
		const F32 height = newpos.mV[VZ];
		const F32 min_height = LLWorld::getInstance()->getMinAllowedZ(mObject, mObject->getPositionGlobal());
		const F32 max_height = LLWorld::getInstance()->getRegionMaxHeight();

		if ( height < min_height)
		{
			newpos.mV[VZ] = min_height;
			mCtrlPosZ->set( min_height );
		}
		else if ( height > max_height )
		{
			newpos.mV[VZ] = max_height;
			mCtrlPosZ->set( max_height );
		}

		// Grass is always drawn on the ground, so clamp its position to the ground
		if (mObject->getPCode() == LL_PCODE_LEGACY_GRASS)
		{
			mCtrlPosZ->set(LLWorld::getInstance()->resolveLandHeightAgent(newpos) + 1.f);
		}
		// Make sure new position is in a valid region, so the object
		// won't get dumped by the simulator.
		new_pos_global = regionp->getPosGlobalFromRegion(newpos);
	}

	// ## Zi: Building spin controls for attachments
	// partly copied from llmaniptranslate.cpp to get the positioning right
	if (mObject->isAttachment())
	{
		LLVector3 old_position_local=mObject->getPosition();

		if(mRootObject!=mObject)
		{
			newpos=newpos-mRootObject->getPosition();
			newpos=newpos*~mRootObject->getRotation();
			mObject->setPositionParent(newpos);
		}
		else
			mObject->setPosition(newpos);
		LLManip::rebuild(mObject) ;

		gAgentAvatarp->clampAttachmentPositions();

		LLVector3 new_position_local = mObject->getPosition();

		// for individually selected roots, we need to counter-translate all unselected children
		if (mObject->isRootEdit())
		{
			// counter-translate child objects if we are moving the root as an individual
			mObject->resetChildrenPosition(old_position_local-new_position_local,TRUE);
		}

		if(!btn_down)
		{
			LLSelectMgr::getInstance()->sendMultipleUpdate(UPD_POSITION);
		}

		LLSelectMgr::getInstance()->updateSelectionCenter();
	}
	// ## Zi: Building spin controls for attachments
	else if (LLWorld::getInstance()->positionRegionValidGlobal(new_pos_global) )
	{
		// send only if the position is changed, that is, the delta vector is not zero
		LLVector3d old_pos_global = mObject->getPositionGlobal();
		LLVector3d delta = new_pos_global - old_pos_global;
		// moved more than 1/20 millimeter
		if (delta.magVec() >= 0.00005f)
		{			
			if (mRootObject != mObject)
			{
				newpos = newpos - mRootObject->getPositionRegion();
				newpos = newpos * ~mRootObject->getRotationRegion();
				mObject->setPositionParent(newpos);
			}
			else
			{
				mObject->setPositionEdit(newpos);
			}			
			
			LLManip::rebuild(mObject) ;

			// for individually selected roots, we need to counter-translate all unselected children
			if (mObject->isRootEdit())
			{								
				// only offset by parent's translation
				mObject->resetChildrenPosition(LLVector3(-delta), TRUE) ;				
			}

			if(!btn_down)
			{
				LLSelectMgr::getInstance()->sendMultipleUpdate(UPD_POSITION);
			}

			LLSelectMgr::getInstance()->updateSelectionCenter();
		}
	}
	else
	{
		// move failed, so we update the UI with the correct values
		LLVector3 vec = mRootObject->getPositionRegion();
		mCtrlPosX->set(vec.mV[VX]);
		mCtrlPosY->set(vec.mV[VY]);
		mCtrlPosZ->set(vec.mV[VZ]);
	}
}

void LLPanelObject::sendSculpt()
{
	if (mObject.isNull())
		return;
	
	LLSculptParams sculpt_params;

	if (mCtrlSculptTexture)
		sculpt_params.setSculptTexture(mCtrlSculptTexture->getImageAssetID());

	U8 sculpt_type = 0;
	
	if (mCtrlSculptType)
		sculpt_type |= mCtrlSculptType->getValue().asInteger();

	bool enabled = sculpt_type != LL_SCULPT_TYPE_MESH;

	if (mCtrlSculptMirror)
	{
		mCtrlSculptMirror->setEnabled(enabled ? TRUE : FALSE);
	}
	if (mCtrlSculptInvert)
	{
		mCtrlSculptInvert->setEnabled(enabled ? TRUE : FALSE);
	}
	
	if ((mCtrlSculptMirror) && (mCtrlSculptMirror->get()))
		sculpt_type |= LL_SCULPT_FLAG_MIRROR;

	if ((mCtrlSculptInvert) && (mCtrlSculptInvert->get()))
		sculpt_type |= LL_SCULPT_FLAG_INVERT;
	
	sculpt_params.setSculptType(sculpt_type);
	mObject->setParameterEntry(LLNetworkData::PARAMS_SCULPT, sculpt_params, TRUE);
}

void LLPanelObject::refresh()
{
	getState();
	if (mObject.notNull() && mObject->isDead())
	{
		mObject = NULL;
	}

	if (mRootObject.notNull() && mRootObject->isDead())
	{
		mRootObject = NULL;
	}
	
	F32 max_scale = get_default_max_prim_scale(LLPickInfo::isFlora(mObject));

	getChild<LLSpinCtrl>("Scale X")->setMaxValue(max_scale);
	getChild<LLSpinCtrl>("Scale Y")->setMaxValue(max_scale);
	getChild<LLSpinCtrl>("Scale Z")->setMaxValue(max_scale);
}


void LLPanelObject::draw()
{
	const LLColor4	white(	1.0f,	1.0f,	1.0f,	1);
	const LLColor4	red(	1.0f,	0.25f,	0.f,	1);
	const LLColor4	green(	0.f,	1.0f,	0.f,	1);
	const LLColor4	blue(	0.f,	0.5f,	1.0f,	1);

	// Tune the colors of the labels
	LLTool* tool = LLToolMgr::getInstance()->getCurrentTool();

	if (tool == LLToolCompTranslate::getInstance())
	{
		mCtrlPosX	->setLabelColor(red);
		mCtrlPosY	->setLabelColor(green);
		mCtrlPosZ	->setLabelColor(blue);

		mCtrlScaleX	->setLabelColor(white);
		mCtrlScaleY	->setLabelColor(white);
		mCtrlScaleZ	->setLabelColor(white);

		mCtrlRotX	->setLabelColor(white);
		mCtrlRotY	->setLabelColor(white);
		mCtrlRotZ	->setLabelColor(white);
	}
	else if ( tool == LLToolCompScale::getInstance() )
	{
		mCtrlPosX	->setLabelColor(white);
		mCtrlPosY	->setLabelColor(white);
		mCtrlPosZ	->setLabelColor(white);

		mCtrlScaleX	->setLabelColor(red);
		mCtrlScaleY	->setLabelColor(green);
		mCtrlScaleZ	->setLabelColor(blue);

		mCtrlRotX	->setLabelColor(white);
		mCtrlRotY	->setLabelColor(white);
		mCtrlRotZ	->setLabelColor(white);
	}
	else if ( tool == LLToolCompRotate::getInstance() )
	{
		mCtrlPosX	->setLabelColor(white);
		mCtrlPosY	->setLabelColor(white);
		mCtrlPosZ	->setLabelColor(white);

		mCtrlScaleX	->setLabelColor(white);
		mCtrlScaleY	->setLabelColor(white);
		mCtrlScaleZ	->setLabelColor(white);

		mCtrlRotX	->setLabelColor(red);
		mCtrlRotY	->setLabelColor(green);
		mCtrlRotZ	->setLabelColor(blue);
	}
	else
	{
		mCtrlPosX	->setLabelColor(white);
		mCtrlPosY	->setLabelColor(white);
		mCtrlPosZ	->setLabelColor(white);

		mCtrlScaleX	->setLabelColor(white);
		mCtrlScaleY	->setLabelColor(white);
		mCtrlScaleZ	->setLabelColor(white);

		mCtrlRotX	->setLabelColor(white);
		mCtrlRotY	->setLabelColor(white);
		mCtrlRotZ	->setLabelColor(white);
	}

	LLPanel::draw();
}

// virtual
void LLPanelObject::clearCtrls()
{
	LLPanel::clearCtrls();

	mCheckLock		->set(FALSE);
	mCheckLock		->setEnabled( FALSE );
	mCheckPhysics	->set(FALSE);
	mCheckPhysics	->setEnabled( FALSE );
	mCheckTemporary	->set(FALSE);
	mCheckTemporary	->setEnabled( FALSE );
	mCheckPhantom	->set(FALSE);
	mCheckPhantom	->setEnabled( FALSE );
	
	// Disable text labels
	mLabelPosition	->setEnabled( FALSE );
	mLabelSize		->setEnabled( FALSE );
	mLabelRotation	->setEnabled( FALSE );
	mLabelCut		->setEnabled( FALSE );
	mLabelHollow	->setEnabled( FALSE );
	mLabelHoleType	->setEnabled( FALSE );
	mLabelTwist		->setEnabled( FALSE );
	mLabelSkew		->setEnabled( FALSE );
	mLabelShear		->setEnabled( FALSE );
	mLabelTaper		->setEnabled( FALSE );
	mLabelRadiusOffset->setEnabled( FALSE );
	mLabelRevolutions->setEnabled( FALSE );

	getChildView("select_single")->setVisible( FALSE);
	getChildView("edit_object")->setVisible( TRUE);	
	getChildView("edit_object")->setEnabled(FALSE);
	
	getChildView("scale_hole")->setEnabled(FALSE);
	getChildView("scale_taper")->setEnabled(FALSE);
	getChildView("advanced_cut")->setEnabled(FALSE);
	getChildView("advanced_dimple")->setEnabled(FALSE);
	getChildView("advanced_slice")->setVisible( FALSE);
}

//
// Static functions
//

// static
void LLPanelObject::onCommitLock(LLUICtrl *ctrl, void *data)
{
	// Checkbox will have toggled itself
	LLPanelObject *self = (LLPanelObject *)data;

	if(self->mRootObject.isNull()) return;

	BOOL new_state = self->mCheckLock->get();
	
	LLSelectMgr::getInstance()->selectionSetObjectPermissions(PERM_OWNER, !new_state, PERM_MOVE | PERM_MODIFY);
}

// static
void LLPanelObject::onCommitPosition( LLUICtrl* ctrl, void* userdata )
{
	LLPanelObject* self = (LLPanelObject*) userdata;
	BOOL btn_down = ((LLSpinCtrl*)ctrl)->isMouseHeldDown() ;
	self->sendPosition(btn_down);
}

// static
void LLPanelObject::onCommitScale( LLUICtrl* ctrl, void* userdata )
{
	LLPanelObject* self = (LLPanelObject*) userdata;
	BOOL btn_down = ((LLSpinCtrl*)ctrl)->isMouseHeldDown() ;
	self->sendScale(btn_down);
}

// static
void LLPanelObject::onCommitRotation( LLUICtrl* ctrl, void* userdata )
{
	LLPanelObject* self = (LLPanelObject*) userdata;
	BOOL btn_down = ((LLSpinCtrl*)ctrl)->isMouseHeldDown() ;
	self->sendRotation(btn_down);
}

// static
void LLPanelObject::onCommitPhysics( LLUICtrl* ctrl, void* userdata )
{
	LLPanelObject* self = (LLPanelObject*) userdata;
	self->sendIsPhysical();
}

// static
void LLPanelObject::onCommitTemporary( LLUICtrl* ctrl, void* userdata )
{
	LLPanelObject* self = (LLPanelObject*) userdata;
	self->sendIsTemporary();
}

// static
void LLPanelObject::onCommitPhantom( LLUICtrl* ctrl, void* userdata )
{
	LLPanelObject* self = (LLPanelObject*) userdata;
	self->sendIsPhantom();
}

void LLPanelObject::onSelectSculpt(const LLSD& data)
{
    LLTextureCtrl* mTextureCtrl = getChild<LLTextureCtrl>("sculpt texture control");

	if (mTextureCtrl)
	{
		mSculptTextureRevert = mTextureCtrl->getImageAssetID();
	}
	
	sendSculpt();
}


void LLPanelObject::onCommitSculpt( const LLSD& data )
{
	sendSculpt();
}

BOOL LLPanelObject::onDropSculpt(LLInventoryItem* item)
{
    LLTextureCtrl* mTextureCtrl = getChild<LLTextureCtrl>("sculpt texture control");

	if (mTextureCtrl)
	{
		LLUUID asset = item->getAssetUUID();

		mTextureCtrl->setImageAssetID(asset);
		mSculptTextureRevert = asset;
	}

	return TRUE;
}


void LLPanelObject::onCancelSculpt(const LLSD& data)
{
	LLTextureCtrl* mTextureCtrl = getChild<LLTextureCtrl>("sculpt texture control");
	if(!mTextureCtrl)
		return;
	
	mTextureCtrl->setImageAssetID(mSculptTextureRevert);
	
	sendSculpt();
}

// static
void LLPanelObject::onCommitSculptType(LLUICtrl *ctrl, void* userdata)
{
	LLPanelObject* self = (LLPanelObject*) userdata;

	self->sendSculpt();
}


void copy_vector_to_clipboard(const LLVector3& vec)
{
	std::string stringVec = llformat("<%g, %g, %g>", vec.mV[VX], vec.mV[VY], vec.mV[VZ]);
	LLView::getWindow()->copyTextToClipboard(utf8str_to_wstring(stringVec));
}

void LLPanelObject::onCopyPos(const LLSD& data)
{
	mClipboardPos = LLVector3(mCtrlPosX->get(), mCtrlPosY->get(), mCtrlPosZ->get());
	copy_vector_to_clipboard(mClipboardPos);
	mBtnPastePos->setToolTip(llformat("Paste Position\n<%g, %g, %g>", mClipboardPos.mV[VX], mClipboardPos.mV[VY], mClipboardPos.mV[VZ]));
	mHasPosClipboard = TRUE;
}

void LLPanelObject::onCopySize(const LLSD& data)
{
	mClipboardSize = LLVector3(mCtrlScaleX->get(), mCtrlScaleY->get(), mCtrlScaleZ->get());
	copy_vector_to_clipboard(mClipboardSize);
	mBtnPasteSize->setToolTip(llformat("Paste Size\n<%g, %g, %g>", mClipboardSize.mV[VX], mClipboardSize.mV[VY], mClipboardSize.mV[VZ]));
	mHasSizeClipboard = TRUE;
}

void LLPanelObject::onCopyRot(const LLSD& data)
{
	mClipboardRot = LLVector3(mCtrlRotX->get(), mCtrlRotY->get(), mCtrlRotZ->get());
	copy_vector_to_clipboard(mClipboardRot);
	mBtnPasteRot->setToolTip(llformat("Paste Rotation\n<%g, %g, %g>", mClipboardRot.mV[VX], mClipboardRot.mV[VY], mClipboardRot.mV[VZ]));
	mHasRotClipboard = TRUE;
}


void LLPanelObject::onPastePos(const LLSD& data)
{
	if(!mHasPosClipboard) return;

	//clamp pos on non-attachments, just keep the prims on the sim
	if (!mObject->isAttachment())
	{
// <FS:CR> Aurora Sim
		//mClipboardPos.mV[VX] = llclamp( mClipboardPos.mV[VX], 0.f, 256.f);
		//mClipboardPos.mV[VY] = llclamp( mClipboardPos.mV[VY], 0.f, 256.f);
		mClipboardPos.mV[VX] = llclamp( mClipboardPos.mV[VX], 0.f, gAgent.getRegion()->getWidth());
		mClipboardPos.mV[VY] = llclamp( mClipboardPos.mV[VY], 0.f, gAgent.getRegion()->getWidth());
// </FS:CR> Aurora Sim
		//height will get properly clammed by sendPosition
	}

	mCtrlPosX->set( mClipboardPos.mV[VX] );
	mCtrlPosY->set( mClipboardPos.mV[VY] );
	mCtrlPosZ->set( mClipboardPos.mV[VZ] );

	LLCalc* calcp = LLCalc::getInstance();
	calcp->setVar(LLCalc::X_POS, mClipboardPos.mV[VX]);
	calcp->setVar(LLCalc::Y_POS, mClipboardPos.mV[VY]);
	calcp->setVar(LLCalc::Z_POS, mClipboardPos.mV[VZ]);

	sendPosition(FALSE);
}

void LLPanelObject::onPasteSize(const LLSD& data)
{
	if(!mHasSizeClipboard) return;
// <AW: opensim-limits>
//	mClipboardSize.mV[VX] = llclamp(mClipboardSize.mV[VX], MIN_PRIM_SCALE, llpanelobject_max_prim_scale());
//	mClipboardSize.mV[VY] = llclamp(mClipboardSize.mV[VY], MIN_PRIM_SCALE, llpanelobject_max_prim_scale());
//	mClipboardSize.mV[VZ] = llclamp(mClipboardSize.mV[VZ], MIN_PRIM_SCALE, llpanelobject_max_prim_scale());
	mClipboardSize.mV[VX] = llclamp(mClipboardSize.mV[VX], mMinScale, llpanelobject_max_prim_scale());
	mClipboardSize.mV[VY] = llclamp(mClipboardSize.mV[VY], mMinScale, llpanelobject_max_prim_scale());
	mClipboardSize.mV[VZ] = llclamp(mClipboardSize.mV[VZ], mMinScale, llpanelobject_max_prim_scale());
// </AW: opensim-limits>

	mCtrlScaleX->set( mClipboardSize.mV[VX] );
	mCtrlScaleY->set( mClipboardSize.mV[VY] );
	mCtrlScaleZ->set( mClipboardSize.mV[VZ] );

	LLCalc* calcp = LLCalc::getInstance();
	calcp->setVar(LLCalc::X_SCALE, mClipboardSize.mV[VX]);
	calcp->setVar(LLCalc::Y_SCALE, mClipboardSize.mV[VY]);
	calcp->setVar(LLCalc::Z_SCALE, mClipboardSize.mV[VZ]);

	sendScale(FALSE);
}

void LLPanelObject::onPasteRot(const LLSD& data)
{
	if(!mHasRotClipboard) return;
	
	mCtrlRotX->set( mClipboardRot.mV[VX] );
	mCtrlRotY->set( mClipboardRot.mV[VY] );
	mCtrlRotZ->set( mClipboardRot.mV[VZ] );

	LLCalc* calcp = LLCalc::getInstance();
	calcp->setVar(LLCalc::X_ROT, mClipboardRot.mV[VX]);
	calcp->setVar(LLCalc::Y_ROT, mClipboardRot.mV[VY]);
	calcp->setVar(LLCalc::Z_ROT, mClipboardRot.mV[VZ]);

	sendRotation(FALSE);
}

//Paste from clip board
BOOL get_vector_from_clipboard(LLVector3* value)
{
	LLWString temp_string;
	LLView::getWindow()->pasteTextFromClipboard(temp_string);
	const std::string stringVec = wstring_to_utf8str(temp_string);

	if(stringVec.empty() || value == NULL) return FALSE;

	LLVector3 vec;
	S32 count = sscanf( stringVec.c_str(), "<%f, %f, %f>", vec.mV + 0, vec.mV + 1, vec.mV + 2 );
	if( 3 == count )
	{
		value->setVec( vec );
		return TRUE;
	}

	return FALSE;
}
void LLPanelObject::onPastePosClip(const LLSD& data)
{
	if(get_vector_from_clipboard(&mClipboardPos))
	{
		mHasPosClipboard = TRUE;
		onPastePos(data);
	}
	else
	{
		llinfos << "Couldn't get position vector from clipboard" << llendl;
	}
}
void LLPanelObject::onPasteSizeClip(const LLSD& data)
{
	if(get_vector_from_clipboard(&mClipboardSize))
	{
		mHasSizeClipboard = TRUE;
		onPasteSize(data);
	}
	else
	{
		llinfos << "Couldn't get size vector from clipboard" << llendl;
	}
}
void LLPanelObject::onPasteRotClip(const LLSD& data)
{
	if(get_vector_from_clipboard(&mClipboardRot))
	{
		mHasRotClipboard = TRUE;
		onPasteRot(data);
	}
	else
	{
		llinfos << "Couldn't get rotation vector from clipboard" << llendl;
	}
}


void LLPanelObject::onCopyParams(const LLSD& data)
{
	getVolumeParams(mClipboardVolumeParams);
	mHasParamClipboard = TRUE;
	
	LLViewerObject* objectp = mObject;
	if (!objectp)
		return;

	LLVOVolume *volobjp = NULL;
	if ( objectp && (objectp->getPCode() == LL_PCODE_VOLUME))
		volobjp = (LLVOVolume *)objectp;
	
	mHasFlexiParam = FALSE;
	if(volobjp && volobjp->isFlexible())
	{
		LLFlexibleObjectData *attributes = (LLFlexibleObjectData *)objectp->getParameterEntry(LLNetworkData::PARAMS_FLEXIBLE);
		if (attributes)
		{
			mPramsClipboard["lod"] = attributes->getSimulateLOD();
			mPramsClipboard["gav"] = attributes->getGravity();
			mPramsClipboard["ten"] = attributes->getTension();
			mPramsClipboard["fri"] = attributes->getAirFriction();
			mPramsClipboard["sen"] = attributes->getWindSensitivity();
			LLVector3 force = attributes->getUserForce();
			mPramsClipboard["forx"] = force.mV[0];
			mPramsClipboard["fory"] = force.mV[1];
			mPramsClipboard["forz"] = force.mV[2];
			mHasFlexiParam = TRUE;
		}
	}

	if (objectp->getParameterEntryInUse(LLNetworkData::PARAMS_SCULPT))
	{
		LLSculptParams *sculpt_params = (LLSculptParams *)objectp->getParameterEntry(LLNetworkData::PARAMS_SCULPT);

		LLUUID image_id = sculpt_params->getSculptTexture();
		BOOL allow_texture = FALSE;
		if (gInventory.isObjectDescendentOf(image_id, gInventory.getLibraryRootFolderID())
			|| image_id == LLUUID(gSavedSettings.getString( "DefaultObjectTexture" ))
			|| image_id == LLUUID(gSavedSettings.getString( "UIImgWhiteUUID" ))
			|| image_id == LLUUID(gSavedSettings.getString( "UIImgInvisibleUUID" ))
			|| image_id == LLUUID(SCULPT_DEFAULT_TEXTURE)
		)
			allow_texture = TRUE;
		else
		{
			LLUUID inventory_item_id;
			LLViewerInventoryCategory::cat_array_t cats;
			LLViewerInventoryItem::item_array_t items;
			LLAssetIDMatches asset_id_matches(image_id);
			gInventory.collectDescendentsIf(LLUUID::null,
									cats,
									items,
									LLInventoryModel::INCLUDE_TRASH,
									asset_id_matches);

			if (items.count())
			{
				// search for copyable version first
				for (S32 i = 0; i < items.count(); i++)
				{
					LLInventoryItem* itemp = items[i];
					LLPermissions item_permissions = itemp->getPermissions();
					if (item_permissions.allowCopyBy(gAgent.getID(), gAgent.getGroupID()))
					{
						inventory_item_id = itemp->getUUID();
						break;
					}
				}
			}
			if (inventory_item_id.notNull())
			{
				LLInventoryItem* itemp = gInventory.getItem(inventory_item_id);
				if (itemp)
				{
					LLPermissions perm = itemp->getPermissions();
					if ( (perm.getMaskBase() & PERM_ITEM_UNRESTRICTED) == PERM_ITEM_UNRESTRICTED )
						allow_texture = TRUE;
				}
			}
		}
		if (allow_texture)
			mPramsClipboard["sculptid"] = image_id;
		else
			mPramsClipboard["sculptid"] = LLUUID(SCULPT_DEFAULT_TEXTURE);

		mPramsClipboard["sculpt_type"] = sculpt_params->getSculptType();
		mHasSculptParam = TRUE;
	}
	else
	{
		mHasSculptParam = FALSE;
	}

	if (volobjp && volobjp->getIsLight())
	{
		mPramsClipboard["Light Intensity"] = volobjp->getLightIntensity();
		mPramsClipboard["Light Radius"] = volobjp->getLightRadius();
		mPramsClipboard["Light Falloff"] = volobjp->getLightFalloff();
		LLColor3 color = volobjp->getLightColor();
		mPramsClipboard["r"] = color.mV[0];
		mPramsClipboard["g"] = color.mV[1];
		mPramsClipboard["b"] = color.mV[2];
		mHasLightParam = TRUE;
	}
	else
	{
		mHasLightParam = FALSE;
	}

}

void LLPanelObject::onPasteParams(const LLSD& data)
{
	LLViewerObject* objectp = mObject;
	if (!objectp)
		return;

	if (mHasFlexiParam && (objectp->getPCode() == LL_PCODE_VOLUME))
	{
		LLFlexibleObjectData *attributes = (LLFlexibleObjectData *)objectp->getParameterEntry(LLNetworkData::PARAMS_FLEXIBLE);
		if (attributes)
		{
			LLFlexibleObjectData new_attributes;
			new_attributes = *attributes;

			new_attributes.setSimulateLOD(mPramsClipboard["lod"].asInteger());
			new_attributes.setGravity(mPramsClipboard["gav"].asReal());
			new_attributes.setTension(mPramsClipboard["ten"].asReal());
			new_attributes.setAirFriction(mPramsClipboard["fri"].asReal());
			new_attributes.setWindSensitivity(mPramsClipboard["sen"].asReal());
			F32 fx = (F32)mPramsClipboard["forx"].asReal();
			F32 fy = (F32)mPramsClipboard["fory"].asReal();
			F32 fz = (F32)mPramsClipboard["forz"].asReal();
			LLVector3 force(fx,fy,fz);
			new_attributes.setUserForce(force);
			objectp->setParameterEntry(LLNetworkData::PARAMS_FLEXIBLE, new_attributes, true);
		}
	}

	if (mHasSculptParam)
	{
		LLSculptParams sculpt_params;

		if (mPramsClipboard.has("sculptid"))
			sculpt_params.setSculptTexture(mPramsClipboard["sculptid"].asUUID());

		if (mPramsClipboard.has("sculptid"))
			sculpt_params.setSculptType((U8)mPramsClipboard["sculpt_type"].asInteger());

		objectp->setParameterEntry(LLNetworkData::PARAMS_SCULPT, sculpt_params, TRUE);
	}
	else
	{
		LLSculptParams *sculpt_params = (LLSculptParams *)objectp->getParameterEntry(LLNetworkData::PARAMS_SCULPT);
		if (sculpt_params)
			objectp->setParameterEntryInUse(LLNetworkData::PARAMS_SCULPT, FALSE, TRUE);
	}
	
	LLVOVolume *volobjp = NULL;
	if ( objectp && (objectp->getPCode() == LL_PCODE_VOLUME))
		volobjp = (LLVOVolume *)objectp;

	if (volobjp && mHasLightParam)
	{
		volobjp->setIsLight(TRUE);
		volobjp->setLightIntensity((F32)mPramsClipboard["Light Intensity"].asReal());
		volobjp->setLightRadius((F32)mPramsClipboard["Light Radius"].asReal());
		volobjp->setLightFalloff((F32)mPramsClipboard["Light Falloff"].asReal());
		F32 r = (F32)mPramsClipboard["r"].asReal();
		F32 g = (F32)mPramsClipboard["g"].asReal();
		F32 b = (F32)mPramsClipboard["b"].asReal();
		volobjp->setLightColor(LLColor3(r,g,b));
	}
	
	if(mHasParamClipboard)
		objectp->updateVolume(mClipboardVolumeParams);
}