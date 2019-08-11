// D3D11Adapter.cpp
// KlayGE D3D11适配器 头文件
// Ver 3.8.0
// 版权所有(C) 龚敏敏, 2009
// Homepage: http://www.klayge.org
//
// 3.8.0
// 初次建立 (2009.1.30)
//
// 修改记录
/////////////////////////////////////////////////////////////////////////////////

#include <KlayGE/KlayGE.hpp>

#include <algorithm>
#include <cstring>
#include <boost/assert.hpp>

#include <KlayGE/D3D11/D3D11Adapter.hpp>

namespace KlayGE
{
	// 构造函数
	/////////////////////////////////////////////////////////////////////////////////
	D3D11Adapter::D3D11Adapter(uint32_t adapter_no, IDXGIAdapter1* adapter)
					: adapter_no_(adapter_no)
	{
		this->ResetAdapter(adapter);
	}

	// 获取设备描述字符串
	/////////////////////////////////////////////////////////////////////////////////
	std::wstring const D3D11Adapter::Description() const
	{
		return std::wstring(adapter_desc_.Description);
	}

	// 获取支持的显示模式数目
	/////////////////////////////////////////////////////////////////////////////////
	size_t D3D11Adapter::NumVideoMode() const
	{
		return modes_.size();
	}

	// 获取显示模式
	/////////////////////////////////////////////////////////////////////////////////
	D3D11VideoMode const & D3D11Adapter::VideoMode(size_t index) const
	{
		BOOST_ASSERT(index < modes_.size());

		return modes_[index];
	}

	// 枚举显示模式
	/////////////////////////////////////////////////////////////////////////////////
	void D3D11Adapter::Enumerate()
	{
		static DXGI_FORMAT constexpr formats[] =
		{
			DXGI_FORMAT_R8G8B8A8_UNORM,
			DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
			DXGI_FORMAT_B8G8R8A8_UNORM,
			DXGI_FORMAT_B8G8R8A8_UNORM_SRGB,
			DXGI_FORMAT_R10G10B10A2_UNORM
		};

		UINT i = 0;
		com_ptr<IDXGIOutput> output;
		while (adapter_->EnumOutputs(i, output.release_and_put()) != DXGI_ERROR_NOT_FOUND)
		{
			if (output != nullptr)
			{
				for (auto const & format : formats)
				{
					UINT num = 0;
					output->GetDisplayModeList(format, DXGI_ENUM_MODES_SCALING, &num, 0);
					if (num > 0)
					{
						std::vector<DXGI_MODE_DESC> mode_descs(num);
						output->GetDisplayModeList(format, DXGI_ENUM_MODES_SCALING, &num, &mode_descs[0]);

						for (auto const & mode_desc : mode_descs)
						{
							D3D11VideoMode const video_mode(mode_desc.Width, mode_desc.Height,
								mode_desc.Format);

							// 如果找到一个新模式, 加入模式列表
							if (std::find(modes_.begin(), modes_.end(), video_mode) == modes_.end())
							{
								modes_.push_back(video_mode);
							}
						}
					}
				}
			}

			++ i;
		}

		std::sort(modes_.begin(), modes_.end());
	}

	void D3D11Adapter::ResetAdapter(IDXGIAdapter1* ada)
	{
		adapter_.reset(ada);
		adapter_->GetDesc1(&adapter_desc_);
		modes_.resize(0);

		if (auto adapter2 = adapter_.as<IDXGIAdapter2>(IID_IDXGIAdapter2))
		{
			DXGI_ADAPTER_DESC2 desc2;
			adapter2->GetDesc2(&desc2);
			memcpy(adapter_desc_.Description, desc2.Description, sizeof(desc2.Description));
			adapter_desc_.VendorId = desc2.VendorId;
			adapter_desc_.DeviceId = desc2.DeviceId;
			adapter_desc_.SubSysId = desc2.SubSysId;
			adapter_desc_.Revision = desc2.Revision;
			adapter_desc_.DedicatedVideoMemory = desc2.DedicatedVideoMemory;
			adapter_desc_.DedicatedSystemMemory = desc2.DedicatedSystemMemory;
			adapter_desc_.SharedSystemMemory = desc2.SharedSystemMemory;
			adapter_desc_.AdapterLuid = desc2.AdapterLuid;
			adapter_desc_.Flags = desc2.Flags;
		}
	}
}
