/******************************************************************************
    Copyright (C) 2013 by Hugh Bailey <obs.jim@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#include <util/base.h>
#include "d3d11-subsystem.hpp"

void gs_texture_3d::InitSRD(vector<D3D11_SUBRESOURCE_DATA> &srd)
{
	uint32_t rowSizeBytes  = width  * gs_get_format_bpp(format);
	uint32_t texSizeBytes  = height * rowSizeBytes / 8;
	size_t   textures      = 1;
	uint32_t actual_levels = levels;
	size_t   curTex = 0;

	if (!actual_levels)
		actual_levels = 1;// gs_get_total_levels(width, height);

	rowSizeBytes /= 8;

	for (size_t i = 0; i < textures; i++) {
		uint32_t newRowSize = rowSizeBytes;
		uint32_t newTexSize = texSizeBytes;

		for (uint32_t j = 0; j < actual_levels; j++) {
			D3D11_SUBRESOURCE_DATA newSRD;
			newSRD.pSysMem          = data[curTex++].data();
			newSRD.SysMemPitch      = newRowSize;
			newSRD.SysMemSlicePitch = newTexSize;
			srd.push_back(newSRD);

			newRowSize /= 2;
			newTexSize /= 4;
		}
	}
}

void gs_texture_3d::BackupTexture(const uint8_t **data)
{
	this->data.resize(levels);

	uint32_t w = width;
	uint32_t h = height;
	uint32_t d = depth;
	uint32_t bbp = gs_get_format_bpp(format);

	for (uint32_t i = 0; i < levels; i++) {
		if (!data[i])
			break;

		uint32_t texSize = bbp * w * h * d/ 8;
		this->data[i].resize(texSize);

		vector<uint8_t> &subData = this->data[i];
		memcpy(&subData[0], data[i], texSize);

		w /= 2;
		h /= 2;
		d /= 2;
	}
}

void gs_texture_3d::InitTexture(const uint8_t **data)
{
	HRESULT hr;

	memset(&td, 0, sizeof(td));
	td.Width            = width;
	td.Height           = height;
	td.Depth			= depth;
	td.MipLevels        = genMipmaps ? 0 : levels;
	//td.ArraySize        = type == GS_TEXTURE_CUBE ? 6 : 1;
	td.Format           = dxgiFormat;
	td.BindFlags        = D3D11_BIND_SHADER_RESOURCE;
	//td.SampleDesc.Count = 1;
	td.CPUAccessFlags   = isDynamic ? D3D11_CPU_ACCESS_WRITE : 0;
	td.Usage            = isDynamic ? D3D11_USAGE_DYNAMIC :
	                                  D3D11_USAGE_DEFAULT;

	if (isRenderTarget || isGDICompatible)
		td.BindFlags |= D3D11_BIND_RENDER_TARGET;

	if (isGDICompatible)
		td.MiscFlags |= D3D11_RESOURCE_MISC_GDI_COMPATIBLE;

	if (data) {
		BackupTexture(data);
		InitSRD(srd);

	}

	hr = device->device->CreateTexture3D(&td, data ? srd.data() : NULL,
			texture.Assign());
	if (FAILED(hr))
		throw HRError("Failed to create 3D texture d3d11-texture3d.cpp", hr);

	if (isGDICompatible) {
		hr = texture->QueryInterface(__uuidof(IDXGISurface1),
				(void**)gdiSurface.Assign());
		if (FAILED(hr))
			throw HRError("Failed to create GDI surface d3d11-texture3d.cpp", hr);
	}
}

void gs_texture_3d::InitResourceView()
{
	HRESULT hr;

	memset(&resourceDesc, 0, sizeof(resourceDesc));
	resourceDesc.Format = dxgiFormat;

	if (type == GS_TEXTURE_CUBE) {
		resourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		resourceDesc.TextureCube.MipLevels = genMipmaps ? -1 : 1;
	} else {
		resourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
		resourceDesc.Texture3D.MipLevels = genMipmaps ? -1 : 1;
	}

	hr = device->device->CreateShaderResourceView(texture, &resourceDesc,
			shaderRes.Assign());
	if (FAILED(hr))
		throw HRError("Failed to create resource view d3d11-texture3d.cpp", hr);
}

void gs_texture_3d::InitRenderTargets()
{
	HRESULT hr;
	hr = device->device->CreateRenderTargetView(texture, NULL,
			renderTarget[0].Assign());
	if (FAILED(hr))
		throw HRError("Failed to create render target view d3d11-texture3d.cpp",
				hr);
}

gs_texture_3d::gs_texture_3d(gs_device_t *device, uint32_t width,
		uint32_t height, uint32_t depth, gs_color_format colorFormat, uint32_t levels,
		const uint8_t **data, uint32_t flags,
		bool gdiCompatible, bool shared)
	: gs_texture      (device, gs_type::gs_texture_3d, GS_TEXTURE_3D, levels,
	                   colorFormat),
	  width           (width),
	  height          (height),
	  depth			  (depth),
	  dxgiFormat      (ConvertGSTextureFormat(format)),
	  isRenderTarget  ((flags & GS_RENDER_TARGET) != 0),
	  isGDICompatible (gdiCompatible),
	  isDynamic       ((flags & GS_DYNAMIC) != 0),
	  isShared        (shared),
	  genMipmaps      ((flags & GS_BUILD_MIPMAPS) != 0)
{
	InitTexture(data);
	InitResourceView();

	if (isRenderTarget)
		InitRenderTargets();
}

gs_texture_3d::gs_texture_3d(gs_device_t *device, uint32_t handle)
	: gs_texture      (device, gs_type::gs_texture_3d,
	                   GS_TEXTURE_3D),
	  isShared        (true),
	  sharedHandle    (handle)
{
	HRESULT hr;
	hr = device->device->OpenSharedResource((HANDLE)(uintptr_t)handle,
			__uuidof(ID3D11Texture3D), (void**)texture.Assign());
	if (FAILED(hr))
		throw HRError("Failed to open shared 3D texture d3d11-texture3d.cpp", hr);

	texture->GetDesc(&td);

	this->type       = GS_TEXTURE_3D;
	this->format     = ConvertDXGITextureFormat(td.Format);
	this->levels     = 1;
	this->device     = device;

	this->width      = td.Width;
	this->height     = td.Height;
	this->depth		 = td.Depth;
	this->dxgiFormat = td.Format;

	memset(&resourceDesc, 0, sizeof(resourceDesc));
	resourceDesc.Format              = td.Format;
	resourceDesc.ViewDimension       = D3D11_SRV_DIMENSION_TEXTURE3D;
	resourceDesc.Texture3D.MipLevels = 1;

	hr = device->device->CreateShaderResourceView(texture, &resourceDesc,
			shaderRes.Assign());
	if (FAILED(hr))
		throw HRError("Failed to create shader resource view d3d11-texture3d.cpp", hr);
}
