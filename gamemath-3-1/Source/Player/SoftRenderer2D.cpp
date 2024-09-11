
#include "Precompiled.h"
#include "SoftRenderer.h"
#include <random>
using namespace CK::DD;

// 격자를 그리는 함수
void SoftRenderer::DrawGizmo2D()
{
	auto& r = GetRenderer();
	const auto& g = Get2DGameEngine();

	// 그리드 색상
	LinearColor gridColor(LinearColor(0.8f, 0.8f, 0.8f, 0.3f));

	// 뷰의 영역 계산
	Vector2 viewPos = g.GetMainCamera().GetTransform().GetPosition();
	Vector2 extent = Vector2(_ScreenSize.X * 0.5f, _ScreenSize.Y * 0.5f);

	// 좌측 하단에서부터 격자 그리기
	int xGridCount = _ScreenSize.X / _Grid2DUnit;
	int yGridCount = _ScreenSize.Y / _Grid2DUnit;

	// 그리드가 시작되는 좌하단 좌표 값 계산
	Vector2 minPos = viewPos - extent;
	Vector2 minGridPos = Vector2(ceilf(minPos.X / (float)_Grid2DUnit), ceilf(minPos.Y / (float)_Grid2DUnit)) * (float)_Grid2DUnit;
	ScreenPoint gridBottomLeft = ScreenPoint::ToScreenCoordinate(_ScreenSize, minGridPos - viewPos);

	for (int ix = 0; ix < xGridCount; ++ix)
	{
		r.DrawFullVerticalLine(gridBottomLeft.X + ix * _Grid2DUnit, gridColor);
	}

	for (int iy = 0; iy < yGridCount; ++iy)
	{
		r.DrawFullHorizontalLine(gridBottomLeft.Y - iy * _Grid2DUnit, gridColor);
	}

	ScreenPoint worldOrigin = ScreenPoint::ToScreenCoordinate(_ScreenSize, -viewPos);
	r.DrawFullHorizontalLine(worldOrigin.Y, LinearColor::Red);
	r.DrawFullVerticalLine(worldOrigin.X, LinearColor::Green);
}

// 게임 오브젝트 목록


// 최초 씬 로딩을 담당하는 함수
void SoftRenderer::LoadScene2D()
{
	// 최초 씬 로딩에서 사용하는 모듈 내 주요 레퍼런스
	auto& g = Get2DGameEngine();

}

// 게임 로직과 렌더링 로직이 공유하는 변수
// 변수 선언 영역
Vector2 currentPosition(100.f, 100.f);
float currentScale = 10.f;
float currentDegree = 0.1f;

// 게임 로직을 담당하는 함수
void SoftRenderer::Update2D(float InDeltaSeconds)
{
	// 게임 로직에서 사용하는 모듈 내 주요 레퍼런스
	auto& g = Get2DGameEngine();
	const InputManager& input = g.GetInputManager();

	// 게임 로직의 로컬 변수
	static float moveSpeed = 100.f;
	static float scaleMin = 5.f;	// 최소 크기 값을 지정하는 변수
	static float scaleMax = 20.f;	// 최대 크기 값을 지정하는 변수
	static float scaleSpeed = 20.f;	// 입력에 따른 크기 변화 속도를 지정하는 변수
	static float rotateSpeed = 180.f;


	// X축과 Y축 입력을 결합해 입력 벡터를 생성한다.
	// X, Y축을 동시에 입력받게 되면 벡터 크기 값이 급성장함으로 정규화시킨다. GetNormalize()
	Vector2 inputVector = Vector2(input.GetAxis(InputAxis::XAxis), input.GetAxis(InputAxis::YAxis)).GetNormalize();
	Vector2 deltaPosition = inputVector * moveSpeed * InDeltaSeconds;
	float deltaScale = input.GetAxis(InputAxis::ZAxis) * scaleSpeed * InDeltaSeconds;	// Z축 입력에 따른 크기의 변화량을 계산한다.
	float deltaDegree = input.GetAxis(InputAxis::WAxis) * rotateSpeed * InDeltaSeconds;


	//물체의 최종 상태 설정
	currentPosition += deltaPosition;
	currentDegree += deltaDegree;

}

// 렌더링 로직을 담당하는 함수
void SoftRenderer::Render2D()
{
	// 렌더링 로직에서 사용하는 모듈 내 주요 레퍼런스
	auto& r = GetRenderer();	// 화면의 시각적 표현을 위해 렌더러 모듈의 래퍼런스를 가져옴
	const auto& g = Get2DGameEngine();

	// 배경에 격자 그리기
	DrawGizmo2D();

	// 렌더링 로직의 로컬 변수
	float rad = 0.f;
	static float increment = 0.001f;
	static std::vector<Vector2> hearts;
	HSVColor hsv(0.f, 1.f, 0.85f);
	// 하트 구성하는 점 생성
	if(hearts.empty())
	{
	
		for (rad = 0.f; rad < Math::TwoPI; rad += increment)
		{
			float sin = sinf(rad);
			float cos = cosf(rad);
			float cos2 = cosf(2 * rad);
			float cos3 = cosf(3 * rad);
			float cos4 = cosf(4 * rad);
			float x = 16.f * sin * sin * sin;
			float y = 10 * cos - 5 * cos2 - 2 * cos3 - cos4;
			hearts.push_back(Vector2(x, y));

		}

	}

	float sin = 0.f; float cos = 0.f;
	// currentDegree sin, cos 얻어낸다. 
	Math::GetSinCos(sin, cos, currentDegree);
	// 각 값을 초기화 후 색상을 증가시키면서 점에 대응
	rad = 0.f;

	for (auto const& v : hearts)
	{
		// 소프트 웨어가 제공하는 HSV 모델의 H는 라디안 값이 아닌 [0.1] 범위 값을 사용한다. 따라서 원의 라디언 값에 해당하는 범위로 변경한다.
		// 1. 점에 크기를 적용한다
		Vector2 scaledV = v * currentScale;
		// 2. 크기가 변한 점을 회전시킴
		Vector2 rotatedV = Vector2(scaledV.X * cos - scaledV.Y * sin, scaledV.X * sin + scaledV.Y * cos);
		// 3. 회전 시킨 점 이동
		Vector2 transltedV = rotatedV + currentPosition;
		hsv.H / rad / Math::TwoPI;
		r.DrawPoint(transltedV, hsv.ToLinearColor());

	}

}

// 메시를 그리는 함수
void SoftRenderer::DrawMesh2D(const class DD::Mesh& InMesh, const Matrix3x3& InMatrix, const LinearColor& InColor)
{
}

// 삼각형을 그리는 함수
void SoftRenderer::DrawTriangle2D(std::vector<DD::Vertex2D>& InVertices, const LinearColor& InColor, FillMode InFillMode)
{
}
