#include "pch.hpp"

#include "IOHandler.hpp"

#include "MyApp.hpp"

#include "ShaderManager.hpp"
#include "MyFrame.hpp"

#include "Renderer.hpp"
#include "RenderTextUtility.hpp"

void IOHandler::OnCanvasSizeChange() {
	Renderer& r = *Frame->renderer;

	r.UpdateSize();
}

void IOHandler::UpdateMouseIndex(const wxPoint& Pos) {

	Renderer& r = *Frame->renderer;

	Eigen::Vector3f NormalizedMouse{
		(Pos.x / r.CanvasSize.x() - 0.5f) * 2.0f,
		((r.CanvasSize.y() - Pos.y) / r.CanvasSize.y() - 0.5f) * 2.0f,
		1.0 };

	Eigen::Vector3f Product = r.ViewProjectionMatrix * NormalizedMouse;
	Eigen::Vector2f Transformed = Product.head<2>();
	auto LastMouseIndex = MouseIndex;
	MouseIndex = r.CoordToNearestPoint(Transformed);

	/*if (state == State::Dragging) {
		MouseIndex = { 0, 0 };
	}*/

	if (LastMouseIndex == MouseIndex)return;

	r.UpdateMouseIndex(MouseIndex);
}

IOHandler::IOHandler(MyFrame* Frame) : Frame(Frame) {}

void IOHandler::MoveViewUpdate() {
	const Eigen::Vector2f Speed = Keyboarddata.Shift ? Eigen::Vector2f{ 100, 100 } : Eigen::Vector2f{ 25, 25 };
	Eigen::Vector2f Diff{ 0,0 };

	if (Keyboarddata.W) Diff.y() -= Speed.y();
	if (Keyboarddata.A) Diff.x() += Speed.x();
	if (Keyboarddata.S) Diff.y() += Speed.y();
	if (Keyboarddata.D) Diff.x() -= Speed.x();

	Renderer& r = *Frame->renderer;

	if (Diff != Eigen::Vector2f::Zero()) {
		r.Dirty = true;
	}
	r.Offset += 2.0 * Diff * r.Zoom;

	if (Keyboarddata.E) {

		Eigen::Vector2f Centre = r.CanvasSize / 2.0;

		Eigen::Vector2f PosD = Centre - Keyboarddata.MousePosition;
		PosD.y() *= -1;

		r.Offset += 2.0 * PosD * r.Zoom;

		// Zoom anpassen
		if (!Keyboarddata.Shift) {
			r.Zoom *= 0.9f;
		}
		else {
			r.Zoom *= 1.1f;
		}

		r.CheckZoomDots();

		r.Offset -= 2.0 * PosD * r.Zoom;
		r.Dirty = true;
	}

	if (Keyboarddata.W || Keyboarddata.A || Keyboarddata.S || Keyboarddata.D || Keyboarddata.E) {
		r.UpdateViewProjectionMatrix();
		UpdateMouseIndex({ (int)Keyboarddata.MousePosition.x(),(int)Keyboarddata.MousePosition.y() });
		r.Render();
	}


	/*if (Keyboarddata.Shift && state == Deleting) {
		DeleteUnderCursor();
	}*/
}

void IOHandler::SetState(const State& state) {
	//if (this->state == State::Placing) {
		//NextPlaceBuilding.reset();
	//}
	this->state = state;
	//if (this->state == Placing) {
		//BottomupMenu.Show(true);
	//}
}

std::string IOHandler::GetStateString() const {
	switch (state) {
	case Normal:
		return "Normal";
	case Dragging:
		return "Dragging";
	case PlacingLine:
		return "PlacingLine";
	case DraggingWhilePlacingLine:
		return "DraggingWhilePlacingLine";
	}
	return "Unknown";
}

void IOHandler::DoLoop() {
	MoveViewUpdate();
	//TextSDFManager::Instance().ProcesOrRenderSthAtMostNTimes(2);
	//FrameGuiUpdate();
	//Render();
	//FrameCount++;
	//if (FrameCount % 100 == 0 || FrameCount % 100 == 50) {
	//Dirty = true;
	//Render();

	//std::stringstream ss;
	//ss << "ViewRect " << ViewRect << " MouseIndex " << MouseIndex.x() << " " << MouseIndex.y();

	//return ss.str();
	//}
}

void IOHandler::OnMouseWheel(const wxPoint& wxPos, double Rotation) {

	Renderer& r = *Frame->renderer;

	Eigen::Vector2f Pos = { wxPos.x,wxPos.y };
	Eigen::Vector2f Centre = r.CanvasSize / 2.0;

	Eigen::Vector2f PosD = Centre - Pos;
	PosD.y() *= -1;

	r.Offset += 2.0 * PosD * r.Zoom;

	r.Zoom *= std::pow(1.1f, -Rotation / 60.0f);

	r.CheckZoomDots();

	r.Offset -= 2.0 * PosD * r.Zoom;

	// Matrix aktualisieren
	r.UpdateViewProjectionMatrix();
	UpdateMouseIndex({ (int)Keyboarddata.MousePosition.x(),(int)Keyboarddata.MousePosition.y() });

	r.Dirty = true;
	r.Render();
}

void IOHandler::OnMouseMove(const wxPoint& wxPos) {
	/*
	{ -1.0, -1.0 },
	{ -1.0,1.0 },
	{ 1.0,-1.0 },
	{ 1.0,1.0 },

	*/

	Keyboarddata.MousePosition = { wxPos.x,wxPos.y };


	UpdateMouseIndex(wxPos);


	//SetTitle(ss.str());

	if (state == State::Dragging || state == State::DraggingWhilePlacingLine) {
		Click = false;
		Renderer& r = *Frame->renderer;

		wxPoint LastOrigin = DraggingOrigin;
		DraggingOrigin = wxPos;

		wxPoint Diff = DraggingOrigin - LastOrigin;
		r.Offset.x() += 2.0 * Diff.x * r.Zoom;
		r.Offset.y() -= 2.0 * Diff.y * r.Zoom;

		r.UpdateViewProjectionMatrix();
		UpdateMouseIndex({ (int)Keyboarddata.MousePosition.x(),(int)Keyboarddata.MousePosition.y() });
		r.Dirty = true;
		r.Render();
	}
}

void IOHandler::OnMouseDown(const wxPoint& wxPos) {
	//if (ClickMenu(wxPos))return;

	//PlaceNextPlaceBuilding();
	//DeleteUnderCursor();

	UpdateMouseIndex(wxPos);
	if (state == PlacingLine) {
		SetState(DraggingWhilePlacingLine);
	}
	else {
		SetState(State::Dragging);
	}
	Click = true;
	DraggingOrigin = wxPos;
}

void IOHandler::OnMouseUp(const wxPoint& wxPos){
	StoppDragg();
	if (Click) {
		Click = false;
		if (state == State::Normal) {
			SetState(State::PlacingLine);
			Frame->CurrentBlock->StartDrag(MouseIndex);
			return;
		}
		if (state == State::PlacingLine) {
			if (Frame->CurrentBlock->AddDrag(MouseIndex)) {
				Frame->CurrentBlock->EndDrag();
				SetState(State::Normal);	
			}
			Renderer& r = *Frame->renderer;
			r.Dirty = true;
			r.Render();
			return;
		}
	}
}

void IOHandler::StoppDragg() {
	if (state == State::Dragging) {
		SetState(State::Normal);
	}
	else if(state == State::DraggingWhilePlacingLine){
		SetState(State::PlacingLine);
	}
	DraggingOrigin = { 0,0 };
	UpdateMouseIndex({ (int)Keyboarddata.MousePosition.x(),(int)Keyboarddata.MousePosition.y() });
}

void IOHandler::OnKeyDown(bool Shift, const int& KeyCode) {

	Keyboarddata.Shift = Shift;
	switch (KeyCode) {
	case 'W':
		Keyboarddata.W = true;
		break;
	case 'A':
		Keyboarddata.A = true;
		break;
	case 'S':
		Keyboarddata.S = true;
		break;
	case 'D':
		Keyboarddata.D = true;
		break;
	case 'E':
		Keyboarddata.E = true;
		break;
	case 'F':
		Keyboarddata.F = true;
		break;
	}
}

void IOHandler::OnKeyUp(bool Shift, const int& KeyCode) {

	Keyboarddata.Shift = Shift;
	switch (KeyCode) {
	case 'W':
		Keyboarddata.W = false;
		break;
	case 'A':
		Keyboarddata.A = false;
		break;
	case 'S':
		Keyboarddata.S = false;
		break;
	case 'D':
		Keyboarddata.D = false;
		break;
	case 'E':
		Keyboarddata.E = false;
		break;
	case 'F':
		Keyboarddata.F = false;
		break;
	}
}

void IOHandler::OnChar(const int& KeyCode) {

	/*auto SelectNthButton = [this](const size_t n) {
		BottomupMenu.Show(true);
		if (BottomupMenu.SelectButtonByIndex(n)) {
			App->ContextBinder->BindContext(App->GlContext.get());
			BottomupMenu.Render();
		}
		};*/

	switch (KeyCode) {
	case WXK_ESCAPE:
		if (state == State::PlacingLine) {
			Frame->CurrentBlock->EndDrag();
			SetState(State::Normal);

			Renderer& r = *Frame->renderer;
			r.Dirty = true;
			r.Render();
			return;
		}
	case WXK_RETURN:
		if (state == State::Normal) {
			SetState(State::PlacingLine);
			Frame->CurrentBlock->StartDrag(MouseIndex);
			return;
		}
		if (state == State::PlacingLine) {
			if (Frame->CurrentBlock->AddDrag(MouseIndex)) {
				Frame->CurrentBlock->EndDrag();
				SetState(State::Normal);
			}
			Renderer& r = *Frame->renderer;
			r.Dirty = true;
			r.Render();
			return;
		}
#ifdef UseCollisionGrid
	case 'I':
		VisualBlock::BoxSize++;
		//Frame->CurrentBlock->CollisionMap.clear();
		break;
	case 'K':
		VisualBlock::BoxSize--;
		//Frame->CurrentBlock->CollisionMap.clear();
		break;
#endif
		/*case WXK_SPACE:
			BottomupMenu.Show(!BottomupMenu.IsShown());
			BottomupMenu.UnselectAll();
			App->ContextBinder->BindContext(App->GlContext.get());
			BottomupMenu.Render();
			break;
		case WXK_ESCAPE:
			BottomupMenu.Show(false);
			BottomupMenu.UnselectAll();
			App->ContextBinder->BindContext(App->GlContext.get());
			BottomupMenu.Render();
			SetState(Normal);
			break;*/
			/*case '1':
				SelectNthButton(0);
				break;
			case '2':
				SelectNthButton(1);
				break;
			case '3':
				SelectNthButton(2);
				break;
			case '4':
				SelectNthButton(3);
				break;
			case '5':
				SelectNthButton(4);
				break;
			case '6':
				SelectNthButton(5);
				break;
			case '7':
				SelectNthButton(6);
				break;
			case '8':
				SelectNthButton(7);
				break;
			case '9':
				SelectNthButton(8);
				break;*/
				//case '0':
				/*case WXK_BACK:
				case WXK_DELETE:
					SelectNthButton(9);
					break;*/
					//case 'Q':
						//PickNextPlaceBuilding();
						//board->
						//break;
					//case 'R':Rotate Next Place Building;
						//RotateNextPlaceBuilding();
						//break;
					//case 'F':
						//PlaceNextPlaceBuilding();
						//DeleteUnderCursor();
						//break;
					//case 'T':
						//FlipNextPlaceBuilding();
	}
}