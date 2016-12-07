{THIS FILE CONTAINS THE CODE THAT'S SUPPOSED TO USED ON THE GROUPEDIT_TAB}

procedure TfmMain.ListBox_groupsSelectionChange(Sender: TObject; User: boolean);
var
  jData: TJSONData;
  jObj: TJSONObject;
  i: Integer;
begin
  if (ListBox_groups.SelCount<>1) then begin
    ListBox_users.Enabled := False;
    ListBox_users.Clear;
  end else begin
    jData := FJSON.FindPath('groups.' + ListBox_groups.Items[ListBox_groups.ItemIndex]);
    if (Assigned(jData)) then begin
      if (jData.JSONType = jtObject) then begin
        jData := TJSONObject(jData).FindPath('users');
        if (Assigned(jData)) then begin
          if (jData.JSONType = jtObject) then begin
            jObj := TJSONObject(jData);
            for i:=0 to (jObj.Count-1) do begin
                jData := jObj.Items[i];
                if (jData.JSONType = jtObject) then
                ListBox_users.AddItem(jObj.Names[i], jData);
            end;
          end;
        end;
      end;
    end;
    ListBox_users.Enabled := True;
  end;
end;

procedure TfmMain.PageControl_mainChange(Sender: TObject);
begin
  Button_back.Visible := False;
  if (PageControl_main.ActivePage = TabSheet_editGroup) then
     Button_back.Visible := True;
end;
