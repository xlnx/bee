#include "bee.h"

using namespace bee;
using namespace bee::gl;
using namespace std;
using namespace glm;

constexpr auto modelWhite = 0x00;
constexpr auto modelBlack = 0x01;
constexpr auto modelPawn = 0x00;
constexpr auto modelRook = 0x02;
constexpr auto modelKnight = 0x04;
constexpr auto modelBishop = 0x06;
constexpr auto modelQueen = 0x08;
constexpr auto modelKing = 0x0A;

Window<4, 2> window("Simple Chess", false, 1024, 1024);

class Chess
{
	static constexpr float gridWidth = 63.333f;
	static constexpr float gridHeight = 63.333f;
public:
	Chess()
	{
		auto camera = scene.create<Viewport>();//<FirstPersonCamera<>>();
		camera->setPosition(0, 2, 3);
		camera->setTarget(0, -0.5, -0.8);
		// CameraCarrier cc(camera);

		auto light = scene.create<PointLight>(vec3(0, 0, 2));
		light->setDiffuseIntensity(2.8f);
		scene.setMajorLight(*light);

		scene.create<ModelObject>("board/board.obj");
		window.dispatch<RenderEvent>(
			[&]() -> bool
			{
				scene.shadowPass();
				scene.cursorPass();
				scene.renderPass();
				return false;
			}
		);
		reset();
	}

	class Pawn: public SelectiveObject<RingObject<ModelObject>>
	{
	public:
		Pawn(int type):
			SelectiveObject<RingObject<ModelObject>>(getModel(type)), 
			player(type & 1), type(type & -2)
		{
		}
		void initPosition(int i, int j)
		{
			this->translate((j - 3.5) * gridWidth, (3.5 - i) * gridHeight, 0);
			this->rotate(0, 0, (!player ? -1 : 1) * ::glm::radians(90.0));
		}
		void move(int di, int dj)
		{
			this->translate(dj * gridWidth, -di * gridHeight, 0);
		}
		void renderSelected(Viewport &viewport) override
		{
			fRingColor = ::glm::vec3(1, 0, 0);
			RingObject<ModelObject>::render(viewport);
		}
		void renderHovered(Viewport &viewport) override
		{
			fRingColor = ::glm::vec3(0, 0, 1);
			RingObject<ModelObject>::render(viewport);
		}
		void renderNormal(Viewport &viewport) override
		{
			ModelObject::render(viewport);
		}
	private:
		static Model &getModel(int type)
		{
			static Model models[] = {
				Model::load("chess/white-pawn.obj"),
				Model::load("chess/black-pawn.obj"),
				Model::load("chess/white-rook.obj"),
				Model::load("chess/black-rook.obj"),
				Model::load("chess/white-knight.obj"),
				Model::load("chess/black-knight.obj"),
				Model::load("chess/white-bishop.obj"),
				Model::load("chess/black-bishop.obj"),
				Model::load("chess/white-queen.obj"),
				Model::load("chess/black-queen.obj"),
				Model::load("chess/white-king.obj"),
				Model::load("chess/black-king.obj"),
			};
			return models[type];
		}
	private:
		int player, type;
	};

	class Grid: public SelectiveObject<ModelObject>
	{
	public:
		Grid(int type):
			type(type)
		{
		}
		void initPosition(int i, int j)
		{
			translate((j - 3.5) * gridWidth, (3.5 - i) * gridHeight, 0);
		}
	private:
		static Model &getModel(int type)
		{
			static Model models[] = {
				Model::load("chess/white-grid.obj"),
				Model::load("chess/black-grid.obj"),
			};
			return models[type];
		}
	private:
		int type;
	};

public:
	void reset() 
	{
		for (int i = 0; i != 8; ++i)
		{
			for (int j = 0; j != 8; ++j)
			{
				if (~initialize[i][j])
				{
					board[i][j] = scene.create<Pawn>(initialize[i][j]);
					board[i][j]->initPosition(i, j);
				}
			}
		}
	}
	void move(int i0, int j0, int i1, int j1)
	{
		if (board[i0][j0].isValid())
		{
			BEE_RAISE(Fatal, "Moving an empty grid.");
		}
		if (board[i1][j1].isValid())
		{
			scene.destroy(board[i1][j1]);
		}
		board[i0][j0]->move(i1 - i0, j1 - j0);
		board[i1][j1] = board[i0][j0];
		board[i0][j0].invalidate();
	}
	void clearBoard()
	{
		for (int i = 0; i != 8; ++i)
		{
			for (int j = 0; j != 8; ++j)
			{
				scene.destroy(board[i][j]);
			}
		}
	}
private:
	static constexpr int initialize[8][8] = {
		modelRook|modelWhite, modelKnight|modelWhite, 
			modelBishop|modelWhite, modelQueen|modelWhite, 
				modelKing|modelWhite, modelBishop|modelWhite,
					modelKnight|modelWhite, modelRook|modelWhite,
		modelPawn|modelWhite, modelPawn|modelWhite, 
			modelPawn|modelWhite, modelPawn|modelWhite, 
				modelPawn|modelWhite, modelPawn|modelWhite, 
					modelPawn|modelWhite, modelPawn|modelWhite, 
		-1, -1, -1, -1, -1, -1, -1, -1, 
		-1, -1, -1, -1, -1, -1, -1, -1, 
		-1, -1, -1, -1, -1, -1, -1, -1, 
		-1, -1, -1, -1, -1, -1, -1, -1, 
		modelPawn|modelBlack, modelPawn|modelBlack, 
			modelPawn|modelBlack, modelPawn|modelBlack, 
				modelPawn|modelBlack, modelPawn|modelBlack, 
					modelPawn|modelBlack, modelPawn|modelBlack, 
		modelRook|modelBlack, modelKnight|modelBlack, 
			modelBishop|modelBlack, modelKing|modelBlack, 
				modelQueen|modelBlack, modelBishop|modelBlack,
					modelKnight|modelBlack, modelRook|modelBlack,
	};
private:
	Scene::Ref<Pawn> board[8][8];
	Scene scene = Scene(0.006f);
};

int Main(int argc, char **argv)
{
	Chess chess;
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	window.dispatchMessages();
	return 0;
}
