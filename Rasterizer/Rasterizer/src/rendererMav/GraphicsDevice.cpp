#include "GraphicsDevice.h"


namespace rendererMav
{
	GraphicsDevice::GraphicsDevice()
	{
	}

	GraphicsDevice::~GraphicsDevice()
	{
	}

	void GraphicsDevice::Initialize(Buffer<Color32>* cb, Buffer<float>* db)
	{
		_bufferColor = cb;
		_bufferDepth = db;
	}

	void GraphicsDevice::Shutdown()
	{
	}

	void GraphicsDevice::Draw(size_t triangleNum)
	{
		VertexInput vinput1, vinput2, vinput3;
		VertexOutput voutput1, voutput2, voutput3;
		for (size_t i = 0; i < triangleNum; i += 3)
		{
			vinput1.Position = _vb[i];
			vinput2.Position = _vb[i + 1];
			vinput3.Position = _vb[i + 2];
			vinput1.Normal = _nb[i];
			vinput2.Normal = _nb[i + 1];
			vinput3.Normal = _nb[i + 2];
			vinput1.Uv = _ub[i];
			vinput2.Uv = _ub[i + 1];
			vinput3.Uv = _ub[i + 2];

			VertexShader(vinput1, voutput1);
			VertexShader(vinput2, voutput2);
			VertexShader(vinput3, voutput3);

			Rasterizer(voutput1, voutput2, voutput3);
		}
	}

	void GraphicsDevice::DrawIndexed(size_t triangleCount, const math::UShort3* ib)
	{
		VertexInput vinput1, vinput2, vinput3;
		VertexOutput voutput1, voutput2, voutput3;
		math::UShort3 id1, id2, id3;
		size_t vertCount = triangleCount * 3;
		for (size_t i = 0; i < vertCount; i += 3)
		{
			vinput1.Position = _vb[ib[i].p];
			vinput2.Position = _vb[ib[i + 1].p];
			vinput3.Position = _vb[ib[i + 2].p];
			vinput1.Normal = _nb[ib[i].n];			
			vinput2.Normal = _nb[ib[i + 1].n];
			vinput3.Normal = _nb[ib[i + 2].n];
			vinput1.Uv = _ub[ib[i].t];
			vinput2.Uv = _ub[ib[i + 1].t];
			vinput3.Uv = _ub[ib[i + 2].t];

			VertexShader(vinput1, voutput1);
			VertexShader(vinput2, voutput2);
			VertexShader(vinput3, voutput3);

			Rasterizer(voutput1, voutput2, voutput3);
		}
	}

	void GraphicsDevice::SetVertexBuffer(const math::Float3 * buf)
	{
		_vb = buf;
	}

	void GraphicsDevice::SetNormalBuffer(const math::Float3 * buf)
	{
		_nb = buf;
	}

	void GraphicsDevice::SetUVBuffer(const math::Float2 * buf)
	{
		_ub = buf;
	}

	void GraphicsDevice::SetWorldViewProjMatrix(const math::Matrix4x4 * m)
	{
		_wvpMat = m;
	}

	void GraphicsDevice::SetWorldMatrix(const math::Matrix4x4 * m)
	{
		_wMat = m;
	}

	void GraphicsDevice::SetWorldInverseTransposeMatrix(const math::Matrix4x4 * m)
	{
		_wInvTransMat = m;
	}

	void GraphicsDevice::SetCameraPosition(const math::Float3* pos)
	{
		_camPos = pos;
	}

	void GraphicsDevice::SetMaterial(const Material * mat)
	{
		_material = mat;
	}

	uint8_t GraphicsDevice::EnableLightAmbient(const Color32 * color)
	{
		if (_lAmbCount < _LIGHT_AMBIENT_COUNT)
		{
			_lightAmb = light::LightAmbient(color);
			++_lAmbCount;
			return _lAmbCount - 1;
		}
		else
		{
			return (uint8_t)-1;
		}
	}

	uint8_t GraphicsDevice::EnableDirectionalLight(const Color32 * col, const math::Float3 * direction)
	{
		if (_lDirCount < _LIGHT_DIRECTIONAL_COUNT)
		{
			_lightsDir[_lDirCount] = light::LightDirectional(col, direction);
			++_lDirCount;
			return _lDirCount - 1;
		}
		else
		{
			return (uint8_t)-1;
		}
	}

	uint8_t GraphicsDevice::EnableSpotLight(const Color32 * col, const math::Float3 * dir, const math::Float3* pos, float attC, float attL, float attQ, float umbra, float penumbra, float falloff)
	{
		if (_lSpotCount < _LIGHT_SPOT_COUNT)
		{
			_lightsSpot[_lSpotCount] = light::LightSpot(col, dir, pos, attC, attL, attQ, umbra, penumbra, falloff);
			++_lSpotCount;
			return _lSpotCount - 1;
		}
		else
		{
			return (uint8_t)-1;
		}
	}

	light::LightAmbient * GraphicsDevice::GetLightAmbient(uint8_t id)
	{
		if (id < _lAmbCount)
		{
			return &_lightAmb;
		}
		else
		{
			return nullptr;
		}
	}

	light::LightDirectional * GraphicsDevice::GetLightDirectional(uint8_t id)
	{
		if (id < _lDirCount)
		{
			return &_lightsDir[id];
		}
		else
		{
			return nullptr;
		}
	}

	light::LightSpot * GraphicsDevice::GetLightSpot(uint8_t id)
	{
		if (id < _lSpotCount)
		{
			return &_lightsSpot[id];
		}
		else
		{
			return nullptr;
		}
	}

	bool GraphicsDevice::DisableLightAmbient(uint8_t id)
	{
		return false;
	}

	bool GraphicsDevice::DisableLightDirectional(uint8_t id)
	{
		return false;
	}

	bool GraphicsDevice::DisableLightSpot(uint8_t id)
	{
		return false;
	}

	bool GraphicsDevice::DisableNextLightAmbient()
	{
		if (_lAmbCount != 0)
		{
			--_lAmbCount;
			return true;
		}
		else
		{
			return false;
		}
	}

	bool GraphicsDevice::DisableNextLightDirectional()
	{
		if (_lDirCount != 0)
		{
			--_lDirCount;
			return true;
		}
		else
		{
			return false;
		}
	}

	bool GraphicsDevice::DisableNextLightSpot()
	{
		if (_lSpotCount != 0)
		{
			--_lSpotCount;
			return true;
		}
		else
		{
			return false;
		}
	}

	int32_t GraphicsDevice::ConvertFromScreenToBuffer(float point, uint16_t maxValue)
	{
		return (int32_t)(point * (float)maxValue * 0.5f + ((float)maxValue * 0.5f));
	}


	void GraphicsDevice::VertexShader(const VertexInput & in, VertexOutput & out)
	{
		math::Float4 outP = math::Float4(in.Position);
		outP = *_wvpMat * outP;
		out.Position = math::Float3(outP / outP.w);

		outP = math::Float4(in.Position);
		outP = *_wMat * outP;
		out.WorldPosition = math::Float3(outP);

		outP = math::Float4(in.Normal);
		outP = *_wInvTransMat * outP;
		out.Normal = math::Float3(outP);
		math::Float3::Normalize(out.Normal);

		out.Uv = in.Uv;
	}

	void GraphicsDevice::Rasterizer(VertexOutput& in1, VertexOutput& in2, VertexOutput& in3)
	{
		PixelInput pi;
		Color32 col;

		int32_t v1x, v1y, v2x, v2y, v3x, v3y;
		v1x = (int32_t)ConvertFromScreenToBuffer(in1.Position.x, _bufferColor->GetWidth());
		v1y = (int32_t)ConvertFromScreenToBuffer(in1.Position.y, _bufferColor->GetHeight());
		v2x = (int32_t)ConvertFromScreenToBuffer(in2.Position.x, _bufferColor->GetWidth());
		v2y = (int32_t)ConvertFromScreenToBuffer(in2.Position.y, _bufferColor->GetHeight());
		v3x = (int32_t)ConvertFromScreenToBuffer(in3.Position.x, _bufferColor->GetWidth());
		v3y = (int32_t)ConvertFromScreenToBuffer(in3.Position.y, _bufferColor->GetHeight());

#ifdef PERSP_CORRECT

		// divide all attribs by z
		in1.Normal /= in1.Position.z;
		in1.Uv /= in1.Position.z;
		in1.WorldPosition /= in1.Position.z;
		in2.Normal /= in2.Position.z;
		in2.Uv /= in2.Position.z;
		in2.WorldPosition /= in2.Position.z;
		in3.Normal /= in3.Position.z;
		in3.Uv /= in3.Position.z;
		in3.WorldPosition /= in3.Position.z;

		// precompute for 1 over z
		in1.Position = 1.0f / in1.Position;
		in2.Position = 1.0f / in2.Position;
		in3.Position = 1.0f / in3.Position;

#endif // PERSP_CORRECT

		// triangle bounding box
		int32_t minX = (min(min(v1x, v2x), v3x));
		int32_t minY = (min(min(v1y, v2y), v3y));
		int32_t maxX = (max(max(v1x, v2x), v3x));
		int32_t maxY = (max(max(v1y, v2y), v3y));

		// screen clipping
		minX = max(minX, 0);
		minY = max(minY, 0);
		maxX = min(maxX, (int32_t)_bufferColor->GetWidth() - 1);
		maxY = min(maxY, (int32_t)_bufferColor->GetHeight() - 1);

		// constant pre-calculation
		int32_t dx2x1 = v2x - v1x;
		int32_t dy2y1 = v2y - v1y;
		int32_t dx3x2 = v3x - v2x;
		int32_t dy3y2 = v3y - v2y;
		int32_t dx1x3 = v1x - v3x;
		int32_t dy1y3 = v1y - v3y;

		// top-left rule booleans
		bool e21isTopLeft = dy2y1 > 0 || (dy2y1 == 0 && dx2x1 > 0);
		bool e32isTopLeft = dy3y2 > 0 || (dy3y2 == 0 && dx3x2 > 0);
		bool e12isTopLeft = dy1y3 > 0 || (dy1y3 == 0 && dx1x3 > 0);

		// baycentric coords data pre-calculation
		float bd00 = Float2Dot((float)dx2x1, (float)dy2y1, (float)dx2x1, (float)dy2y1);
		float bd01 = Float2Dot((float)dx2x1, (float)dy2y1, -(float)dx1x3, -(float)dy1y3);
		float bd11 = Float2Dot(-(float)dx1x3, -(float)dy1y3, -(float)dx1x3, -(float)dy1y3);
		float bdenom = 1.0f / (bd00 * bd11 - bd01 * bd01);

		for (int32_t i = minY; i <= maxY; ++i)
		{
			for (int32_t j = minX; j <= maxX; ++j)
			{
				int32_t e21edgeEquation = (dx2x1 * (i - v1y) - dy2y1 * (j - v1x));
				int32_t e32edgeEquation = (dx3x2 * (i - v2y) - dy3y2 * (j - v2x));
				int32_t e12edgeEquation = (dx1x3 * (i - v3y) - dy1y3 * (j - v3x));
				if (
					(
					((e21edgeEquation < 0) || (e21isTopLeft && e21edgeEquation <= 0)) &&
						((e32edgeEquation < 0) || (e32isTopLeft && e32edgeEquation <= 0)) &&
						((e12edgeEquation < 0) || (e12isTopLeft && e12edgeEquation <= 0))
						) /*||
						  (
						  ((e21edgeEquation > 0) || (e21isTopLeft && e21edgeEquation >= 0)) &&
						  ((e32edgeEquation > 0) || (e32isTopLeft && e32edgeEquation >= 0)) &&
						  ((e12edgeEquation > 0) || (e12isTopLeft && e12edgeEquation >= 0))
						  )*/
					)
				{
					// barycentric coords calculation
					float bv2x = (float)j - (float)v1x;
					float bv2y = (float)i - (float)v1y;
					float bd20 = Float2Dot(bv2x, bv2y, (float)dx2x1, (float)dy2y1);
					float bd21 = Float2Dot(bv2x, bv2y, -(float)dx1x3, -(float)dy1y3);

					float bw = (bd11 * bd20 - bd01 * bd21) * bdenom;
					float bu = (bd00 * bd21 - bd01 * bd20) * bdenom;
					float bv = 1.0f - bw - bu;

					// z-buffer clipping check
					// depth interpolation
					float cDepth = _bufferDepth->GetPixel(j, i);
					float mDepth = in1.Position.z * bv + in2.Position.z * bw + in3.Position.z * bu;
					if (mDepth < cDepth)
					{
						// write z to depth buffer
						_bufferDepth->SetPixel(j, i, mDepth);

						pi.Position = math::Int2(j, i);
						pi.WorldPosition = in1.WorldPosition * bv + in2.WorldPosition * bw + in3.WorldPosition * bu;
						pi.Normal = in1.Normal * bv + in2.Normal * bw + in3.Normal * bu;
						math::Float3::Normalize(pi.Normal);
						pi.Uv = in1.Uv * bv + in2.Uv * bw + in3.Uv * bu;

#ifdef PERSP_CORRECT

						float z = 1.0f / (bw * in1.Position.z + bu * in2.Position.z + bv * in3.Position.z);
						pi.WorldPosition *= z;
						pi.Uv *= z;
						pi.Normal *= z;

#endif // PERSP_CORRECT

						PixelShader(pi, col);

						//col = Color32(1.0f, mDepth, mDepth, mDepth);

						// write output color to buffer
						_bufferColor->SetPixel(j, i, col);
					}
				}
			}
		}
	}

	// save depth here

	void GraphicsDevice::PixelShader(const PixelInput & in, Color32 & out)
	{
		math::Float3 temp;

		math::Float3 camDir = *_camPos - in.WorldPosition;
		temp = *_camPos - in.WorldPosition;
		math::Float3::Normalize(temp);

		math::Float3::Normalize(camDir);
		Color32 color(0xFF000000);
		Color32 tempColor;
		Color32 tex = _material->GetMapDiffuse()->GetColor(in.Uv, math::Float3());
		float dot;
		float spec = 0.0f;
		
		float spotEffect;
		float distance;

		// Directional
		for (size_t i = 0; i < _lDirCount; ++i)
		{
			dot = max(math::Float3::Dot(in.Normal, -*_lightsDir[i].GetDirection()), 0.0f);
			tempColor = *_lightsDir[i].GetColor() * dot;
			color += tempColor;

			temp = camDir - *_lightsDir[i].GetDirection();
			math::Float3::Normalize(temp);
			spec += pow(math::Float3::Dot(temp, in.Normal), _material->GetGlossiness()) * tex.GetFltA() * tempColor.GetAverageNoAlpha();
		}

		// Spot
		for (size_t i = 0; i < _lSpotCount; ++i)
		{
			// direction of light ray
			temp = in.WorldPosition - *_lightsSpot[i].GetPostition();
			math::Float3::Normalize(temp);

			// spotEffect
			spotEffect = math::Float3::Dot(*_lightsSpot[i].GetDirection(), temp);

			// check whether we are out or inside spot area
			if (spotEffect > _lightsSpot[i].GetUmbraAngleRad())
			{
				spotEffect = pow(spotEffect, _lightsSpot[i].GetFalloffFactor());
				
				// attenuation
				distance = math::Float3::LengthSquared(in.WorldPosition - *_lightsSpot[i].GetPostition());
				spotEffect = spotEffect / (_lightsSpot[i].GetAttenuationConstant() +
					_lightsSpot[i].GetAttenuationLinear() * distance * 0.25f +
					_lightsSpot[i].GetAttenuationQuadratic() * distance);

				dot = max(math::Float3::Dot(in.Normal, -temp), 0.0f);
				tempColor = *_lightsSpot[i].GetColor() * dot * spotEffect;
				color += tempColor;

				temp = camDir - *_lightsSpot[i].GetDirection();
				math::Float3::Normalize(temp);
				spec += pow(math::Float3::Dot(temp, in.Normal), _material->GetGlossiness()) * tex.GetFltA() * tempColor.GetAverageNoAlpha();
			}
		}
		
		// Texture (multiply with color)
		color = Color32::MulNoAlpha(color, tex) * *_material->GetColorDiffuse();
		
		// specular addition after texture
		color += *_material->GetColorSpecular() * spec;

		// Ambient
		if (_lAmbCount)
		{
			color += *_lightAmb.GetColor() * *_material->GetColorAmbient();
		}
		out = color;
	}

}