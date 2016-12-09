{INCLUDES ALL UTILITY METHODS}

procedure TfmMain.AddInfo(s: String);
begin
  FInfoList.Add(s);
  if (not GroupBox_infoBalloon.Visible) then begin
     Timer_info.Enabled := True;
     NextInfo;
  end;
end;

function TfmMain.NextInfo: Boolean;
var
  s: String;
begin
  Result := False;
  GroupBox_infoBalloon.Visible := False;
  if (FInfoList.Count > 0) then begin
     s := FInfoList[0];
     FInfoList.Delete(0);
     Memo_info.Text := s;
     GroupBox_infoBalloon.Visible := True;
     Result := True;
  end;
end;

procedure TfmMain.GroupBox_infoBalloonClick(Sender: TObject);
begin
  Timer_infoTimer(GroupBox_infoBalloon);
end;

procedure TfmMain.Timer_infoTimer(Sender: TObject);
begin
  If (not NextInfo) then
     Timer_info.Enabled := False;
end;
