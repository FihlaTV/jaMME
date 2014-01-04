// Heavy Repeater Weapon

#include "cg_local.h"

/*
---------------------------
FX_RepeaterProjectileThink
---------------------------
*/

void FX_RepeaterProjectileThink( centity_t *cent, const struct weaponInfo_s *weapon )
{
	vec3_t forward;

	if ( VectorNormalize2( cent->currentState.pos.trDelta, forward ) == 0.0f )
	{
		forward[2] = 1.0f;
	}

	trap_FX_PlayEffectID( cgs.effects.repeaterProjectileEffect, cent->lerpOrigin, forward, -1, -1 );
}

/*
------------------------
FX_RepeaterHitWall
------------------------
*/

void FX_RepeaterHitWall( vec3_t origin, vec3_t normal )
{
	trap_FX_PlayEffectID( cgs.effects.repeaterWallImpactEffect, origin, normal, -1, -1 );
}

/*
------------------------
FX_RepeaterHitPlayer
------------------------
*/

void FX_RepeaterHitPlayer( vec3_t origin, vec3_t normal, qboolean humanoid )
{
	trap_FX_PlayEffectID( cgs.effects.repeaterFleshImpactEffect, origin, normal, -1, -1 );
}

static void CG_DistortionOrb( centity_t *cent )
{
	refEntity_t ent;
	vec3_t ang;
	float scale = 0.5f;
	float vLen;

	if (!cg_renderToTextureFX.integer)
	{
		return;
	}
	memset( &ent, 0, sizeof( ent ) );

	VectorCopy( cent->lerpOrigin, ent.origin );

	VectorSubtract(ent.origin, cg.refdef.vieworg, ent.axis[0]);
	vLen = VectorLength(ent.axis[0]);
	if (VectorNormalize(ent.axis[0]) <= 0.1f)
	{	// Entity is right on vieworg.  quit.
		return;
	}

//	VectorCopy(cg.refdef.viewaxis[2], ent.axis[2]);
//	CrossProduct(ent.axis[0], ent.axis[2], ent.axis[1]);
	vectoangles(ent.axis[0], ang);
	ang[ROLL] = cent->trickAlpha;
	cent->trickAlpha += 16; //spin the half-sphere to give a "screwdriver" effect
	AnglesToAxis(ang, ent.axis);

	//radius must be a power of 2, and is the actual captured texture size
	if (vLen < 128)
	{
		ent.radius = 256;
	}
	else if (vLen < 256)
	{
		ent.radius = 128;
	}
	else if (vLen < 512)
	{
		ent.radius = 64;
	}
	else
	{
		ent.radius = 32;
	}

	VectorScale(ent.axis[0], scale, ent.axis[0]);
	VectorScale(ent.axis[1], scale, ent.axis[1]);
	VectorScale(ent.axis[2], -scale, ent.axis[2]);

	ent.hModel = cgs.media.halfShieldModel;
	ent.customShader = 0;//cgs.media.halfShieldShader;	

#if 1
	ent.renderfx = (RF_DISTORTION|RF_RGB_TINT);

	//tint the whole thing a shade of blue
	ent.shaderRGBA[0] = 200.0f;
	ent.shaderRGBA[1] = 200.0f;
	ent.shaderRGBA[2] = 255.0f;
#else //no tint
	ent.renderfx = RF_DISTORTION;
#endif

	trap_R_AddRefEntityToScene( &ent );
}


//q3pro/QtZ code
void FX_Mortar_Missile( centity_t *cent, const struct weaponInfo_s *weapon )
{
	refEntity_t ent;
	vec3_t ang;
	float vLen;
	float scale = 1.0f;
	refdef_t *refdef = CG_GetRefdef();

	memset( &ent, 0, sizeof( ent ) );

	VectorCopy( cent->lerpOrigin, ent.origin );

	VectorSubtract( ent.origin, refdef->vieworg, ent.axis[0] );
	vLen = VectorLength( ent.axis[0] );
	if ( VectorNormalize( ent.axis[0] ) <= 0.1f )
		return;

//	VectorCopy(refdef->viewaxis[2], ent.axis[2]);
//	CrossProduct(ent.axis[0], ent.axis[2], ent.axis[1]);
	vectoangles( ent.axis[0], ang );
	ang[0] = cent->trickAlpha;
	cent->trickAlpha += 12; //spin the half-sphere to give a "screwdriver" effect
	AnglesToAxis( ang, ent.axis );

	//radius must be a power of 2, and is the actual captured texture size
//	if (vLen < 128)			ent.radius = 256;
//	else if (vLen < 256)	ent.radius = 128;
//	else if (vLen < 512)	ent.radius = 64;
//	else					ent.radius = 32;
	scale = 0.37f;
	VectorScale( ent.axis[0],  scale, ent.axis[0] );
	VectorScale( ent.axis[1],  scale, ent.axis[1] );
	VectorScale( ent.axis[2], -scale, ent.axis[2] );


	ent.hModel = cgs.media.halfShieldModel;

	ent.shaderRGBA[0] = 128;	ent.shaderRGBA[1] = 43;		ent.shaderRGBA[2] = 255;	ent.shaderRGBA[3] = 48;
	ent.customShader = trap_R_RegisterShaderNoMip( "gfx/effects/shock_ripple" );
	ent.renderfx = (RF_RGB_TINT|RF_FORCE_ENT_ALPHA);
//	SE_R_AddRefEntityToScene( &ent, cent->currentState.number );
	trap_R_AddRefEntityToScene( &ent, cent->currentState.number );

	ent.shaderRGBA[0] = 255;	ent.shaderRGBA[1] = 255;	ent.shaderRGBA[2] = 255;	ent.shaderRGBA[3] = 255;
	ent.customShader	= trap_R_RegisterShaderNoMip( "gfx/effects/caustic1" );
	ent.renderfx		= (RF_RGB_TINT|RF_MINLIGHT);
//	SE_R_AddRefEntityToScene( &ent, cent->currentState.number );
	trap_R_AddRefEntityToScene( &ent, cent->currentState.number );

	scale = (1.15 + Q_fabs((float)sin(cg.time / 80.0 + cg.timeFraction / 80.0))*0.65f);
	VectorScale( ent.axis[0], scale, ent.axis[0] );
	VectorScale( ent.axis[1], scale, ent.axis[1] );
	VectorScale( ent.axis[2], scale, ent.axis[2] );
	ent.shaderRGBA[0] = 51;		ent.shaderRGBA[1] = 119;	ent.shaderRGBA[2] = 255;	ent.shaderRGBA[3] = 255;
	ent.customShader = trap_R_RegisterShaderNoMip( "gfx/effects/eplosion_wave" );
	ent.renderfx = (RF_RGB_TINT|RF_ALPHA_DEPTH);
//	SE_R_AddRefEntityToScene( &ent, cent->currentState.number );
	trap_R_AddRefEntityToScene( &ent, cent->currentState.number );

	trap_R_AddLightToScene( cent->lerpOrigin, 400, 0.13f, 0.43f, 0.87f );
}


/*
------------------------------
FX_RepeaterAltProjectileThink
-----------------------------
*/

void FX_RepeaterAltProjectileThink( centity_t *cent, const struct weaponInfo_s *weapon )
{
	vec3_t forward;

	if ( VectorNormalize2( cent->currentState.pos.trDelta, forward ) == 0.0f )
	{
		forward[2] = 1.0f;
	}

	if ( (cg_newFX.integer & NEWFX_REPEATER_ALT) )
	{
		FX_Mortar_Missile( cent, weapon );
		return;
	}

	if (cg_repeaterOrb.integer)
	{
		CG_DistortionOrb(cent);
	}
	trap_FX_PlayEffectID( cgs.effects.repeaterAltProjectileEffect, cent->lerpOrigin, forward, -1, -1 );
}

/*
------------------------
FX_RepeaterAltHitWall
------------------------
*/

void FX_RepeaterAltHitWall( vec3_t origin, vec3_t normal )
{
	trap_FX_PlayEffectID( cgs.effects.repeaterAltWallImpactEffect, origin, normal, -1, -1 );
}

/*
------------------------
FX_RepeaterAltHitPlayer
------------------------
*/

void FX_RepeaterAltHitPlayer( vec3_t origin, vec3_t normal, qboolean humanoid )
{
	trap_FX_PlayEffectID( cgs.effects.repeaterAltWallImpactEffect, origin, normal, -1, -1 );
}
