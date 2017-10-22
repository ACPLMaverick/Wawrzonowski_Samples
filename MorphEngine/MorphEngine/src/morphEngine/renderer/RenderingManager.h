#pragma once
#include "core/Singleton.h"
#include "utility/MArray.h"
#include "device/GraphicsDevice.h"
#include "debugging/Statistics.h"

namespace morphEngine
{
	namespace renderer
	{
		class RenderTarget2D;

		/// <summary>
		/// Singleton.
		/// </summary>
		class RenderingManager :
			public core::Singleton<RenderingManager>
		{
			friend class core::Singleton<RenderingManager>;

		public:

#pragma region DrawOptionsFlags

			typedef MUint16 DrawOptions;
			static const DrawOptions DRAWOPTIONS_FILL = 0b0000000000000001;
			static const DrawOptions DRAWOPTIONS_MESH = 0b0000000000000010;
			static const DrawOptions DRAWOPTIONS_BOUNDS = 0b0000000000000100;

#pragma endregion

		protected:

#pragma region StructsProtected

			struct RenderTargetResizeData
			{
				MUint16 NewWidth;
				MUint16 NewHeight;

				inline RenderTargetResizeData(MUint16 nWidth = 0, MUint16 nHeight = 0) :
					NewWidth(nWidth),
					NewHeight(nHeight)
				{

				}

				inline void Clear() { NewWidth = 0; NewHeight = 0; }
				inline bool IsValid() { return NewWidth != 0; }
			};

#pragma endregion

#pragma region Protected

			utility::MArray<RenderTarget2D*> _renderTargets;
			RenderTargetResizeData _rtResizeData;

			device::GraphicsDevice* _device;

			DrawOptions _drawOptions = DRAWOPTIONS_FILL;

			memoryManagement::Handle<debugging::Statistics> _stats;

#pragma endregion

#pragma region Functions Protected

			RenderingManager();
			~RenderingManager();

			void ResizeRenderTargets();

#pragma endregion

		public:

#pragma region Functions Public

			void Initialize();
			void Shutdown();

			void Draw();
			void DrawEditor();

			inline device::GraphicsDevice* GetDevice() const { return _device; }
			inline utility::MArray<RenderTarget2D*>& GetRenderTargets() { return _renderTargets; }
			RenderTarget2D* CreateRenderTarget(MUint16 width, MUint16 height, device::GraphicDataFormat format, bool depthBufferEnabled = true,
					device::GraphicDataFormat depthBufferFormat = device::GraphicDataFormat::UNORM_D24_UINT_S8);
			void DestroyRenderTarget(RenderTarget2D* rt);

			void RegisterRenderStatisticsComponent(memoryManagement::Handle<debugging::Statistics> stats);
			void AddSetGlobalCall();
			void AddSetMaterialCall();
			void AddDrawCall(MSize vertices, MSize indices);

			void OnWindowResize(MInt32 nWidth, MInt32 nHeight);

			DrawOptions GetDrawOptions() const { return _drawOptions; }
			bool GetDrawOptionEnabled(DrawOptions option) const { return _drawOptions & option; }
			void AddDrawOption(DrawOptions option) { _drawOptions |= option; }
			void ClearDrawOption(DrawOptions option) { _drawOptions &= ~option; }

#pragma endregion

		};

	}
}