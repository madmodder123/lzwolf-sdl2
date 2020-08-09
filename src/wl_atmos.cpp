#include "version.h"

#include "wl_def.h"
#include "wl_main.h"
#include "wl_draw.h"
#include "wl_agent.h"
#include "wl_play.h"
#include "id_ca.h"
#include "id_vh.h"
#include "g_mapinfo.h"
#include "textures/textures.h"
#include "v_video.h"

#define mapheight (map->GetHeader().height)
#define mapwidth (map->GetHeader().width)

uint32_t rainpos = 0;

typedef struct {
	int32_t x, y, z;
} point3d_t;

#define MAXPOINTS 400
point3d_t points[MAXPOINTS];

byte moon[100]={
	0,  0, 27, 18, 15, 16, 19, 29,  0,  0,
	0, 22, 16, 15, 15, 16, 16, 18, 24,  0,
	27, 17, 15, 17, 16, 16, 17, 17, 18, 29,
	18, 15, 15, 15, 16, 16, 17, 17, 18, 20,
	16, 15, 15, 16, 16, 17, 17, 18, 19, 21,
	16, 15, 17, 20, 18, 17, 18, 18, 20, 22,
	19, 16, 18, 19, 17, 17, 18, 19, 22, 24,
	28, 19, 17, 17, 17, 18, 19, 21, 25, 31,
	0, 23, 18, 19, 18, 20, 22, 24, 28,  0,
	0,  0, 28, 21, 20, 22, 28, 30,  0,  0 };

void Init3DPoints()
{
	int hvheight = viewheight >> 1;
	for(int i = 0; i < MAXPOINTS; i++)
	{
		point3d_t *pt = &points[i];
		pt->x = 16384 - (rand() & 32767);
		pt->z = 16384 - (rand() & 32767);
		float len = sqrt((float)pt->x * pt->x + (float)pt->z * pt->z);
		int j=50;
		do
		{
			pt->y = 1024 + (rand() & 8191);
			j--;
		}
		while(j > 0 && (float)pt->y * 256.F / len >= hvheight);
	}
}

void DrawHighQualityStarSky(byte *vbuf, uint32_t vbufPitch)
{
	int hvheight = viewheight >> 1;
	int hvwidth = viewwidth >> 1;

	byte *ptr = vbuf;
	int i;
	for(i = 0; i < hvheight; i++, ptr += vbufPitch)
		memset(ptr, 0, viewwidth);

	for(i = 0; i < MAXPOINTS; i++)
	{
		point3d_t *pt = &points[i];
		int32_t x = pt->x * viewcos + pt->z * viewsin;
		int32_t y = pt->y << 16;
		int32_t z = (pt->z * viewcos - pt->x * viewsin) >> 8;
		if(z <= 0) continue;
		int shade = z >> 18;
		if(shade > 15) continue;
		int32_t xx = x / z + hvwidth;
		int32_t yy = hvheight - y / z;
		if(xx >= 1 && xx < viewwidth - 1 && yy >= 1 && yy < hvheight - 1)
		{
			vbuf[yy * vbufPitch + xx] = shade + 15;
			if (15 - shade > 1)
			{
				shade = 15 - ((15 - shade) >> 1);
				vbuf[yy * vbufPitch + xx + 1] =
					vbuf[yy * vbufPitch + xx - 1] =
					vbuf[(yy + 1) * vbufPitch + xx] =
					vbuf[(yy - 1) * vbufPitch + xx] = shade + 15;
			}
		}
	}

	FTextureID picnum = TexMan.CheckForTexture ("FULLMOON", FTexture::TEX_Any);
	if(!picnum.isValid())
		return;
	FTexture *picture = TexMan(picnum);

	{
		int pointInd = levelInfo->Atmos[3];
		pointInd = (
			pointInd == 1 || pointInd < 0 || pointInd >= MAXPOINTS) ? 10 : pointInd;

		point3d_t *pt = &points[pointInd];
		int32_t x = pt->x * viewcos + pt->z * viewsin;
		int32_t y = pt->y << 16;
		int32_t z = (pt->z * viewcos - pt->x * viewsin) >> 8;
		if(z <= 0) return;
		int32_t xx = x / z + hvwidth;
		int32_t yy = hvheight - y / z;
		xx += viewscreenx - 15;
		yy += viewscreeny - 15;
		if(xx + 30 >= 0 && xx < (int32_t)screenWidth &&
			yy + 30 >= 0 && yy < (int32_t)screenHeight)
		{
			double x = xx, y = yy;

			screen->Lock(false);
			double wd = picture->GetScaledWidthDouble();
			double hd = picture->GetScaledHeightDouble();

			screen->DrawTexture(picture, x, y,
				DTA_DestWidthF, wd,
				DTA_DestHeightF, hd,
				DTA_ClipLeft, viewscreenx,
				DTA_ClipRight, viewscreenx + viewwidth,
				DTA_ClipTop, viewscreeny,
				DTA_ClipBottom, viewscreeny + viewheight,
				TAG_DONE);
			screen->Unlock();
		}
	}
}

void DrawStarSky(byte *vbuf, uint32_t vbufPitch)
{
	int hvheight = viewheight >> 1;
	int hvwidth = viewwidth >> 1;

	byte *ptr = vbuf;
	int i;
	for(i = 0; i < hvheight; i++, ptr += vbufPitch)
		memset(ptr, 0, viewwidth);

	for(i = 0; i < MAXPOINTS; i++)
	{
		point3d_t *pt = &points[i];
		int32_t x = pt->x * viewcos + pt->z * viewsin;
		int32_t y = pt->y << 16;
		int32_t z = (pt->z * viewcos - pt->x * viewsin) >> 8;
		if(z <= 0) continue;
		int shade = z >> 18;
		if(shade > 15) continue;
		int32_t xx = x / z + hvwidth;
		int32_t yy = hvheight - y / z;
		if(xx >= 0 && xx < viewwidth && yy >= 0 && yy < hvheight)
			vbuf[yy * vbufPitch + xx] = shade + 15;
	}

	int32_t x = 16384 * viewcos + 16384 * viewsin;
	int32_t z = (16384 * viewcos - 16384 * viewsin) >> 8;
	if(z <= 0)
		return;
	int32_t xx = x / z + hvwidth;
	int32_t yy = hvheight - ((hvheight - (hvheight >> 3)) << 22) / z;
	if(xx > -10 && xx < viewwidth)
	{
		int stopx = 10, starty = 0, stopy = 10;
		i = 0;
		if(xx < 0) i = -xx;
		if(xx > viewwidth - 11) stopx = viewwidth - xx;
		if(yy < 0) starty = -yy;
		if(yy > viewheight - 11) stopy = viewheight - yy;
		for(; i < stopx; i++)
			for(int j = starty; j < stopy; j++)
				vbuf[(yy + j) * vbufPitch + xx + i] = moon[j * 10 + i];
	}
}

void DrawRain(byte *vbuf, uint32_t vbufPitch)
{
//#if defined(USE_FLOORCEILINGTEX) && defined(FIXRAINSNOWLEAKS)
	fixed dist;                                // distance to row projection
	fixed tex_step;                            // global step per one screen pixel
	fixed gu, gv, floorx, floory;              // global texture coordinates
//#endif

	fixed px = (players[0].camera->y + FixedMul(0x7900, viewsin)) >> 6;
	fixed pz = (players[0].camera->x - FixedMul(0x7900, viewcos)) >> 6;
	int32_t ax, az, x, y, z, xx, yy, height, actheight;
	int shade;
	int hvheight = viewheight >> 1;
	int hvwidth = viewwidth >> 1;

	rainpos -= tics * 900;
	for(int i = 0; i < MAXPOINTS; i++)
	{
		point3d_t *pt = &points[i];
		ax = pt->x + px;
		ax = 0x1fff - (ax & 0x3fff);
		az = pt->z + pz;
		az = 0x1fff - (az & 0x3fff);
		x = ax * viewcos + az * viewsin;
		y = (((((pt->y << 6) + rainpos) & 0x0ffff) - 0x8000) << 11);
		z = (az * viewcos - ax * viewsin) >> 8;
		if(z <= 0) continue;
		shade = z >> 17;
		if(shade > 13) continue;
		xx = x / z + hvwidth;
		if(xx < 0 || xx >= viewwidth) continue;
		actheight = y / z;
		yy = hvheight - actheight;
		height = (heightnumerator << 10) / z;
		if(actheight < 0) actheight = -actheight;
		if(actheight < (wallheight[xx] >> 3) && height < wallheight[xx]) continue;
		if(xx >= 0 && xx < viewwidth && yy > 0 && yy < viewheight)
		{
//#if defined(USE_FLOORCEILINGTEX) && defined(FIXRAINSNOWLEAKS)
			// Find the rain's tile coordinate
			// NOTE: This sometimes goes over the map edges.
			dist = ((heightnumerator / ((height >> 3) + 1)) << 5);
			gu =  viewx + FixedMul(dist, viewcos);
			gv = -viewy + FixedMul(dist, viewsin);
			floorx = (  gu >> TILESHIFT     ) % mapwidth;
			floory = (-(gv >> TILESHIFT) - 1) % mapheight;

			// Is there a ceiling tile?
			const MapSpot spot = map->GetSpot(floorx, floory, 0);
			if(spot->sector)
			{
				FTextureID curtex = spot->sector->texture[MapSector::Ceiling];
				if (curtex.isValid())
					continue;
			}
//#endif

			vbuf[yy * vbufPitch + xx] = shade+15;
			vbuf[(yy - 1) * vbufPitch + xx] = shade+16;
			if(yy > 2)
				vbuf[(yy - 2) * vbufPitch + xx] = shade+17;
		}
	}
}

void DrawSnow(byte *vbuf, uint32_t vbufPitch)
{
//#if defined(USE_FLOORCEILINGTEX) && defined(FIXRAINSNOWLEAKS)
	fixed dist;                                // distance to row projection
	fixed tex_step;                            // global step per one screen pixel
	fixed gu, gv, floorx, floory;              // global texture coordinates
//#endif

	fixed px = (players[0].camera->y + FixedMul(0x7900, viewsin)) >> 6;
	fixed pz = (players[0].camera->x - FixedMul(0x7900, viewcos)) >> 6;
	int32_t ax, az, x, y, z, xx, yy, height, actheight;
	int shade;
	int hvheight = viewheight >> 1;
	int hvwidth = viewwidth >> 1;

	rainpos -= tics * 256;
	for(int i = 0; i < MAXPOINTS; i++)
	{
		point3d_t *pt = &points[i];
		ax = pt->x + px;
		ax = 0x1fff - (ax & 0x3fff);
		az = pt->z + pz;
		az = 0x1fff - (az & 0x3fff);
		x = ax * viewcos + az * viewsin;
		y = (((((pt->y << 6) + rainpos) & 0x0ffff) - 0x8000) << 11);
		z = (az * viewcos - ax * viewsin) >> 8;
		if(z <= 0) continue;
		shade = z >> 17;
		if(shade > 13) continue;
		xx = x / z + hvwidth;
		if(xx < 0 || xx >= viewwidth) continue;
		actheight = y / z;
		yy = hvheight - actheight;
		height = (heightnumerator << 10) / z;
		if(actheight < 0) actheight = -actheight;
		if(actheight < (wallheight[xx] >> 3) && height < wallheight[xx]) continue;
		if(xx > 0 && xx < viewwidth && yy > 0 && yy < viewheight)
		{
//#if defined(USE_FLOORCEILINGTEX) && defined(FIXRAINSNOWLEAKS)
			// Find the snow's tile coordinate
			// NOTE: This sometimes goes over the map edges.
			dist = ((heightnumerator / ((height >> 3) + 1)) << 5);
			gu =  viewx + FixedMul(dist, viewcos);
			gv = -viewy + FixedMul(dist, viewsin);
			floorx = (  gu >> TILESHIFT     ) % mapwidth;
			floory = (-(gv >> TILESHIFT) - 1) % mapheight;

			// Is there a ceiling tile?
			const MapSpot spot = map->GetSpot(floorx, floory, 0);
			if(spot->sector)
			{
				FTextureID curtex = spot->sector->texture[MapSector::Ceiling];
				if (curtex.isValid())
					continue;
			}
//#endif

			if(shade < 10)
			{
				vbuf[yy * vbufPitch + xx] = shade+17;
				vbuf[yy * vbufPitch + xx - 1] = shade+16;
				vbuf[(yy - 1) * vbufPitch + xx] = shade+16;
				vbuf[(yy - 1) * vbufPitch + xx - 1] = shade+15;
			}
			else
				vbuf[yy * vbufPitch + xx] = shade+15;
		}
	}
}
