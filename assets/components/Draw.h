#pragma once
#include <K_Engine.h>
#include "Mesh.h"

namespace K 
{
	KC_API struct LinePoint
	{
		K::Vector3 position;
		K::Colour colour;
		float thickness;
		bool vertexAdded = false;
		bool indexAdded = false;

		LinePoint();

		LinePoint(K::Vector3 pos, K::Colour col, float thick)
		{
			position = pos;
			colour = col;
			thickness = thick;
		}
	};

	KC_API struct LineArtLine
	{
		std::vector<LinePoint> points;
	};

	KC_API class LineArt
	{
		private:
			std::vector<LineArtLine> lineArtLines;
		public:
			int NumberOfLines();

			void AddLineArtLine(LineArtLine lineArtLine);

			void Render(K::Mesh* mesh);

			void Clear();

			bool IsEar(int index, int nextIndex, int previousIndex);

			std::vector<int> ProcessNodes(std::vector<int> nodes, K::Mesh* mesh);

			bool IsCollidingWithTriangle(K::Vector3 point, K::Vector3 A, K::Vector3 B, K::Vector3 C);

			LineArt();

			~LineArt();
	};

	KC_API class Draw : public K::Component 
	{
	private:
		bool drawingLine = false;
		K::LineArt lineart;
		K::Mesh* mesh;
		bool drawingOverlay = false;
		std::string properties;
		
	public:

		Draw();

		~Draw();

		void DrawPolygon(int sides, float radius);

		void PolygonTest01();

		void PolygonTest02();

		void PolygonTest03();

		void Init() override;

		void Update() override;

		void UpdateEditor() override;

		void Bind() override;

		void Unbind()  override;

		void SetPropertyValues(const char* value, int valueIndex) override;

		const char* GetPropertyValues() override;
	};
}