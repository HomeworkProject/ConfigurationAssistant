{THIS FILE CONTAINS THE CODE THAT'S SUPPOSED TO USED ON THE GROUPEDIT_TAB}

procedure TfmMain.ListBox_groupsSelectionChange(Sender: TObject; User: boolean);
var
  jData: TJSONData;
  jObj: TJSONObject;
  i: Integer;
begin
  ListBox_users.Clear;
  if (ListBox_groups.SelCount<>1) then begin
    ListBox_users.Enabled := False;
    Button_crUser.Enabled := False;
    Button_delGroup.Enabled := False;
    Button_delUser.Enabled := False;
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
    Button_crUser.Enabled := True;
    Button_delGroup.Enabled := True;
  end;
end;

procedure TfmMain.ListBox_usersSelectionChange(Sender: TObject; User: boolean);
begin
  if (ListBox_users.SelCount<>1) then begin
    Button_delUser.Enabled := False;
  end else begin
      Button_delUser.Enabled := True;
  end;
end;

procedure TfmMain.PageControl_mainChange(Sender: TObject);
begin
  Button_back.Visible := False;
  if (PageControl_main.ActivePage = TabSheet_editGroup) then
     Button_back.Visible := True;
end;

procedure TfmMain.Button_crGroupClick(Sender: TObject);
var
  lObj, lObj2: TJSONObject;
  GName: String;
begin
  GName := Edit_GUname.Text;
  if (Assigned(FJSON.FindPath('groups.' + GName))) then begin
    ShowMessage('Group already exists');
    Exit;
  end;

  lObj := FJSON.Objects['groups'];
  lObj2 := TJSONObject.Create;
  lObj2.Add('users', TJSONObject.Create);
  lObj2.Add('name', GName);
  lObj.Add(GName, lObj2);
  reloadFromJSON;
  ListBox_groupsSelectionChange(Button_crGroup, False);
  AddInfo('Group ' + GName + ' created');
end;

procedure TfmMain.Button_crUserClick(Sender: TObject);
var
  lObj: TJSONObject;
  GName, UName: String;
begin
  UName := Edit_GUname.Text;
  GName := ListBox_groups.Items[ListBox_groups.ItemIndex];
  if (Assigned(FJSON.FindPath('groups.' + GName + '.users.' + UName))) then begin
    ShowMessage('User already exists');
    Exit;
  end;

  lObj := FJSON.Objects['groups'].Objects[GName].Objects['users'];
  lObj.Add(UName, TJSONObject.Create);

  reloadFromJSON;
  ListBox_groupsSelectionChange(Button_crGroup, False);
  AddInfo('User ' + GName + '/' + UName + ' created');
end;

procedure TfmMain.Button_delGroupClick(Sender: TObject);
var
  GName: String;
begin
  GName := ListBox_groups.Items[ListBox_groups.ItemIndex];
  if (Assigned(FJSON.FindPath('groups.' + GName))) then begin
    FJSON.Objects['groups'].Delete(GName);
    reloadFromJSON;
    ListBox_groupsSelectionChange(Button_crGroup, False);
    AddInfo('Group ' + GName + ' deleted');
  end;
end;

procedure TfmMain.Button_delUserClick(Sender: TObject);
var
  GName, UName: String;
begin
  GName := ListBox_groups.Items[ListBox_groups.ItemIndex];
  UName := ListBox_users.Items[ListBox_users.ItemIndex];
  if (Assigned(FJSON.FindPath('groups.' + GName + '.users.' + UName))) then begin
    FJSON.Objects['groups'].Objects[GName].Objects['users'].Delete(GName);
    reloadFromJSON;
    ListBox_groupsSelectionChange(Button_crGroup, False);
    AddInfo('User ' + UName + '/' + GName + ' deleted');
  end;
end;
