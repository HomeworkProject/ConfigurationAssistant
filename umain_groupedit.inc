{INCLUDES THE CODE THAT'S SUPPOSED TO USED ON THE GROUPEDIT_TAB}

procedure TfmMain.ListBox_groupsClick(Sender: TObject);
begin
  ListBox_usersSelectionChange(ListBox_groups, False);
end;

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
var
  jData, jData2: TJSONData;
  jObj, jObj2: TJSONObject;
begin
  if (ListBox_users.SelCount<>1) then begin
    Button_delUser.Enabled := False;
    GroupBox_userInfo.Enabled := False;
    Edit_authMethod.Text := 'default';
    Edit_pass.Clear;
    Edit_authData.Clear;
  end else begin
      Button_delUser.Enabled := True;
      GroupBox_userInfo.Enabled := True;
      Edit_authMethod.Text := 'default';
      Edit_pass.Clear;
      Edit_authData.Clear;
      jObj := TJSONObject(ListBox_users.Items.Objects[ListBox_users.ItemIndex]);
      jData := jObj.FindPath('auth');
      if (not Assigned(jData)) then Exit;
      if (not (jData.JSONType = jtObject)) then Exit;
      jObj2 := TJSONObject(jData);
      Edit_authMethod.Text := jObj2.Get('method', 'default');
      Edit_authData.Text := jObj2.Get('pass', '');

      jData := jObj.FindPath('onLoad.passwd');
      if (not Assigned(jData)) then Exit;
      if (not (jData.JSONType = jtObject)) then Exit;
      jObj := TJSONObject(jData);
      if (jObj.Get('password', '') <> '') then
         Edit_pass.Text := jObj.Get('password', '>>NULL<<');
      if (jObj.Get('method', '') <> '') then
         Edit_authMethod.Text := jObj.Get('method', '>>NULL<<');
  end;
end;

function TfmMain.GroupExists(gName: String): Boolean;
begin
  Result := Assigned(FJSON.FindPath('groups.' + GName));
end;

procedure TfmMain.GroupCreate(gName: String);
var
  lObj, lObj2: TJSONObject;
begin
  lObj := FJSON.Objects['groups'];
  lObj.Delete(gName);
  lObj2 := TJSONObject.Create;
  lObj2.Add('users', TJSONObject.Create);
  lObj2.Add('name', GName);
  lObj.Add(GName, lObj2);
  reloadFromJSON;
  ListBox_groupsSelectionChange(Button_crGroup, False);
  FConfigModified := True;
end;

procedure TfmMain.Button_crGroupClick(Sender: TObject);
var
  lObj, lObj2: TJSONObject;
  GName: String;
begin
  GName := Edit_GUname.Text;
  if (GroupExists(GName)) then begin
    ShowMessage('Group already exists');
    Exit;
  end;
  GroupCreate(gName);
  AddInfo('Group ' + GName + ' created');
end;

function TfmMain.UserExists(gName, uName: String): Boolean;
begin
  Result := Assigned(FJSON.FindPath('groups.' + GName + '.users.' + UName));
end;

procedure TfmMain.UserCreate(gName, uName: String);
var
  lObj: TJSONObject;
begin
  lObj := FJSON.Objects['groups'].Objects[GName].Objects['users'];
  lObj.Delete(UName);
  lObj.Add(UName, TJSONObject.Create);

  reloadFromJSON;
  ListBox_groupsSelectionChange(Button_crGroup, False);
  FConfigModified := True;
end;

procedure TfmMain.Button_crUserClick(Sender: TObject);
var
  lObj: TJSONObject;
  GName, UName: String;
begin
  UName := Edit_GUname.Text;
  GName := ListBox_groups.Items[ListBox_groups.ItemIndex];
  if (UserExists(gName, uName)) then begin
    ShowMessage('User already exists');
    Exit;
  end;
  UserCreate(gName, uName);
  AddInfo('User ' + GName + '/' + UName + ' created');
  FConfigModified := True;
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
    FConfigModified := True;
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
    FConfigModified := True;
  end;
end;

procedure TfmMain.Button_GenerateClick(Sender: TObject);
begin
  PageControl_main.ActivePage := TabSheet_genGroupsUsers;
end;

procedure TfmMain.Passwd(user: TJSONObject; method, password: String);
var
  jData: TJSONData;
begin
  if (not Assigned(user.FindPath('onLoad'))) then user.Add('onLoad', TJSONObject.Create);
  if (password <> '') then begin
    jData := user.Objects['onLoad'].FindPath('passwd');
    if (not Assigned(jData) or not (jData.JSONType = jtObject)) then user.Objects['onLoad'].Add('passwd', TJSONObject.Create);
    user.Objects['onLoad'].Objects['passwd'].Delete('password');
    user.Objects['onLoad'].Objects['passwd'].Add('password', password);
    user.Objects['onLoad'].Objects['passwd'].Delete('method');
    user.Objects['onLoad'].Objects['passwd'].Add('method', method);
  end else begin
        user.Objects['onLoad'].Delete('passwd');
  end;
  FConfigModified := True;
end;

procedure TfmMain.Button_applyClick(Sender: TObject);
var
  jData: TJSONData;
  user, jObj: TJSONObject;
begin
  user := TJSONObject(ListBox_users.Items.Objects[ListBox_users.ItemIndex]);
  jData := user.FindPath('onLoad');
  if (not Assigned(jData) or not (jData.JSONType =jtObject)) then user.Add('onLoad', TJSONObject.Create);

  Passwd(user, Edit_authMethod.Text, Edit_pass.Text);

  if (CheckBox_setAdmin.Checked) then begin
    user.Objects['onLoad'].Delete('perm');
    user.Objects['onLoad'].Add('perm', 'admin');
  end else begin
      jData := user.FindPath('onLoad.perm');
      if (Assigned(jData)) then
         user.Objects['onLoad'].Delete('perm');
  end;
  FConfigModified := True;
end;

