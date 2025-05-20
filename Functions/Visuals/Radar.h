#pragma once

#include "../../CSSx64.h"

namespace Radar {

class Base_Radar {
 public:
  // �����״�����
  inline void SetSize(const float& Size);
  inline void SetPos(const ImVec2& Pos);
  inline void SetRange(const float& Range);
  inline void SetCrossColor(const ImColor& Color);
  inline void SetProportion(const float& Proportion);
  inline void SetDrawList(ImDrawList* DrawList);
  // ��ȡ�״�����
  float GetSize();
  ImVec2 GetPos();
  // ��ӻ��Ƶ�
  inline void AddPoint(const Vector3& LocalPos, const float& LocalYaw,
                const Vector3& Pos, ImColor Color, int Type = 0,
                float Yaw = 0.0f);
  // ��Ⱦ
  inline void Render();
  inline void RadarSetting();
 public:
  ImDrawList* DrawList = nullptr;
  // ʮ����ʾ
  bool ShowCrossLine = true;
  // ʮ����ɫ
  ImColor CrossColor = ImColor(255, 255, 255, 255);
  // ����
  float Proportion = 2680;
  // Բ��뾶
  float CircleSize = 4;
  // ��ͷ�ߴ�
  float ArrowSize = 11;
  // Բ����ͷ�ߴ�
  float ArcArrowSize = 7;
  // �״ﷶΧ
  float RenderRange = 250;
  // ����Yaw����
  float LocalYaw = 0.0f;
  // ״̬
  bool Opened = true;
  // �״�������� 0:Բ�� 1:��ͷ 2:Բ����ͷ
  int PointType = 0;

 private:
  ImVec2 Pos{0, 0};
  float Width = 200;
  std::vector<std::tuple<ImVec2, ImColor, int, float>> Points;
};

inline ImVec2 RevolveCoordinatesSystem(float RevolveAngle, ImVec2 OriginPos,
                                ImVec2 DestPos) {
  ImVec2 ResultPos;
  if (RevolveAngle == 0) return DestPos;
  ResultPos.x = OriginPos.x +
                (DestPos.x - OriginPos.x) * cos(M_DEG2RAD(RevolveAngle)) +
                (DestPos.y - OriginPos.y) * sin(M_DEG2RAD(RevolveAngle));
  ResultPos.y = OriginPos.y -
                (DestPos.x - OriginPos.x) * sin(M_DEG2RAD(RevolveAngle)) +
                (DestPos.y - OriginPos.y) * cos(M_DEG2RAD(RevolveAngle));
  return ResultPos;
}
inline void Base_Radar::SetRange(const float& Range) {
  this->RenderRange = Range;
}

inline void Base_Radar::SetCrossColor(const ImColor& Color) {
  this->CrossColor = Color;
}

inline void Base_Radar::SetPos(const ImVec2& Pos) { this->Pos = Pos; }

inline void Base_Radar::SetSize(const float& Size) { this->Width = Size; }

inline float Base_Radar::GetSize() { return this->Width; }

inline ImVec2 Base_Radar::GetPos() { return this->Pos; }

inline void Base_Radar::SetProportion(const float& Proportion) {
  this->Proportion = Proportion;
}

inline void Base_Radar::SetDrawList(ImDrawList* DrawList) {
  this->DrawList = DrawList;
}

inline void Base_Radar::AddPoint(const Vector3& LocalPos, const float& LocalYaw,
                          const Vector3& Pos, ImColor Color, int Type,
                          float Yaw) {
  ImVec2 PointPos;
  float Distance;
  float Angle;

  this->LocalYaw = LocalYaw;

  Distance = sqrt(pow(LocalPos.x - Pos.x, 2) + pow(LocalPos.y - Pos.y, 2));

  Angle = M_RAD2DEG(atan2(Pos.y - LocalPos.y, Pos.x - LocalPos.x));
  Angle = M_DEG2RAD(this->LocalYaw - Angle);

  Distance = Distance / this->Proportion * this->RenderRange * 2;

  PointPos.x = this->Pos.x + Distance * sin(Angle);
  PointPos.y = this->Pos.y - Distance * cos(Angle);

  // Circle range
  // Distance = sqrt(pow(this->Pos.x - PointPos.x, 2) + pow(this->Pos.y -
  // PointPos.y, 2)); if (Distance > this->RenderRange) 	return;

  // Rectangle range
  if (PointPos.x < this->Pos.x - RenderRange ||
      PointPos.x > this->Pos.x + RenderRange ||
      PointPos.y > this->Pos.y + RenderRange ||
      PointPos.y < this->Pos.y - RenderRange)
    return;

  std::tuple<ImVec2, ImColor, int, float> Data(PointPos, Color, Type, Yaw);
  this->Points.push_back(Data);
}

inline void DrawTriangle(ImVec2 Center, ImColor Color, float Width,
                         float Height,
                  float Yaw) {
  ImVec2 a, b, c;
  ImVec2 Re_a, Re_b, Re_c;
  a = ImVec2{Center.x - Width / 2, Center.y};
  b = ImVec2{Center.x + Width / 2, Center.y};
  c = ImVec2{Center.x, Center.y - Height};
  a = RevolveCoordinatesSystem(-Yaw, Center, a);
  b = RevolveCoordinatesSystem(-Yaw, Center, b);
  c = RevolveCoordinatesSystem(-Yaw, Center, c);
  ImGui::GetForegroundDrawList()->AddTriangleFilled(
      ImVec2(a.x, a.y), ImVec2(b.x, b.y), ImVec2(c.x, c.y), Color);
}

inline void Base_Radar::Render() {
  if (Width <= 0) return;

  // Cross
  std::pair<ImVec2, ImVec2> Line1;
  std::pair<ImVec2, ImVec2> Line2;

  Line1.first = ImVec2(this->Pos.x - this->Width / 2, this->Pos.y);
  Line1.second = ImVec2(this->Pos.x + this->Width / 2, this->Pos.y);
  Line2.first = ImVec2(this->Pos.x, this->Pos.y - this->Width / 2);
  Line2.second = ImVec2(this->Pos.x, this->Pos.y + this->Width / 2);

  if (this->Opened) {
    if (this->ShowCrossLine) {
      this->DrawList->AddLine(Line1.first, Line1.second, this->CrossColor, 1);
      this->DrawList->AddLine(Line2.first, Line2.second, this->CrossColor, 1);
    }

    for (auto PointSingle : this->Points) {
      ImVec2 PointPos = std::get<0>(PointSingle);
      ImColor PointColor = std::get<1>(PointSingle);
      int PointType = std::get<2>(PointSingle);
      float PointYaw = std::get<3>(PointSingle);
      if (PointType == 0) {
        // Բ����ʽ
        this->DrawList->AddCircle(PointPos, this->CircleSize, PointColor);
        this->DrawList->AddCircleFilled(PointPos, this->CircleSize,
                                        ImColor(0, 0, 0));
      } else if (PointType == 1) {
        // ��ͷ��ʽ
        ImVec2 a, b, c;
        ImVec2 Re_a, Re_b, Re_c;
        ImVec2 Re_Point;
        float Angle = (this->LocalYaw - PointYaw) + 180;
        Re_Point = RevolveCoordinatesSystem(Angle, this->Pos, PointPos);

        Re_a = ImVec2(Re_Point.x, Re_Point.y + this->ArrowSize);
        Re_b = ImVec2(Re_Point.x - this->ArrowSize / 1.5,
                      Re_Point.y - this->ArrowSize / 2);
        Re_c = ImVec2(Re_Point.x + this->ArrowSize / 1.5,
                      Re_Point.y - this->ArrowSize / 2);

        a = RevolveCoordinatesSystem(-Angle, this->Pos, Re_a);
        b = RevolveCoordinatesSystem(-Angle, this->Pos, Re_b);
        c = RevolveCoordinatesSystem(-Angle, this->Pos, Re_c);

        this->DrawList->AddQuadFilled(ImVec2(a.x, a.y), ImVec2(b.x, b.y),
                                      ImVec2(PointPos.x, PointPos.y),
                                      ImVec2(c.x, c.y), PointColor);
        this->DrawList->AddQuad(ImVec2(a.x, a.y), ImVec2(b.x, b.y),
                                ImVec2(PointPos.x, PointPos.y),
                                ImVec2(c.x, c.y), ImColor(0, 0, 0, 150), 0.1);
      } else {
        // Բ����ͷ
        ImVec2 TrianglePoint, TrianglePoint_1, TrianglePoint_2;
        float Angle = (this->LocalYaw - PointYaw) - 90;

        this->DrawList->AddCircleFilled(PointPos, 0.85 * this->ArcArrowSize,
                                        PointColor, 30);
        this->DrawList->AddCircle(PointPos, 0.95 * this->ArcArrowSize,
                                  ImColor(0, 0, 0, 150), 0, 0.1);

        TrianglePoint.x =
            PointPos.x + (this->ArcArrowSize + this->ArcArrowSize / 3) *
                             cos(M_DEG2RAD(-Angle));
        TrianglePoint.y =
            PointPos.y - (this->ArcArrowSize + this->ArcArrowSize / 3) *
                             sin(M_DEG2RAD(-Angle));

        TrianglePoint_1.x =
            PointPos.x + this->ArcArrowSize * cos(M_DEG2RAD(-(Angle - 30)));
        TrianglePoint_1.y =
            PointPos.y - this->ArcArrowSize * sin(M_DEG2RAD(-(Angle - 30)));

        TrianglePoint_2.x =
            PointPos.x + this->ArcArrowSize * cos(M_DEG2RAD(-(Angle + 30)));
        TrianglePoint_2.y =
            PointPos.y - this->ArcArrowSize * sin(M_DEG2RAD(-(Angle + 30)));

        this->DrawList->PathLineTo(TrianglePoint);
        this->DrawList->PathLineTo(TrianglePoint_1);
        this->DrawList->PathLineTo(TrianglePoint_2);
        this->DrawList->PathFillConvex(ImColor(220, 220, 220, 240));
      }
    }
  }

  if (this->Points.size() > 0) this->Points.clear();
}

inline void Base_Radar::RadarSetting() {
  // Radar window
  ImGui::SetNextWindowBgAlpha(0.f);
  ImGui::SetNextWindowSize({config::RadarRange * 2, config::RadarRange * 2},
                           ImGuiCond_Always);
  ImGui::Begin(XorStr("RADAR"), 0,
               ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground |
                   ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar |
                   ImGuiWindowFlags_::ImGuiWindowFlags_NoBringToFrontOnFocus |
                   ImGuiWindowFlags_::ImGuiWindowFlags_NoBackground |
                   ImGuiWindowFlags_::ImGuiWindowFlags_NoNavFocus |
                   ImGuiWindowFlags_::ImGuiWindowFlags_NoNav);

  // this->SetPos({ Gui.Window.Size.x / 2,Gui.Window.Size.y / 2 });
  this->SetDrawList(ImGui::GetWindowDrawList());
  this->SetPos({ImGui::GetWindowPos().x + config::RadarRange,
                ImGui::GetWindowPos().y + config::RadarRange});
  this->SetProportion(config::RadarProportion);
  this->SetRange(config::RadarRange);
  this->SetSize(config::RadarRange * 2);
  this->SetCrossColor(ImColor(220, 220, 220, 255));

  this->ArcArrowSize *= config::RadarPointSize;
  this->ArrowSize *= config::RadarPointSize;
  this->CircleSize *= config::RadarPointSize;

  this->ShowCrossLine = config::RadarCrossLine;
  this->Opened = true;
}

  static std::unordered_map<uintptr_t, float> g_dormantStartTimes;

  void RadarRun(EntityList & entityList) {
    if (!config::Radar) return;
    Base_Radar Radar;
    Radar.RadarSetting();
    int local_team = entityList.local_player_data.team;

    auto now = std::chrono::steady_clock::now();
    float currentTime =
        std::chrono::duration<float>(now.time_since_epoch()).count();

    for (const auto& ent : entityList.GetCurrentEntities()) {
      if (!ent.IsValid()) continue;
      if (!ent.IsEnemy(local_team)) continue;

      if (ent.data.dormant) {
        if (g_dormantStartTimes.find(ent.index) == g_dormantStartTimes.end()) {
          g_dormantStartTimes[ent.index] = currentTime;
        }

        if (currentTime - g_dormantStartTimes[ent.index] >
            config::RadarDormantTime) {
          continue;
        }
      } else {
        g_dormantStartTimes.erase(ent.index);
      }

      Vector3 vecPos = ent.data.position;

      float eyeAng = ent.data.viewangles.y;

      Radar.AddPoint(entityList.local_player_data.position,
                     entityList.local_player_data.viewangles.y, vecPos,
                     ImColor(237, 85, 106, 200), 2, eyeAng);
    }
    Radar.Render();
    ImGui::End();
  }

}  // namespace Radar