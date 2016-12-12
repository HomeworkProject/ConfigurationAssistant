{INCLUDES THE CODE THAT'S SUPPOSED TO LOAD THE CONFIG}

procedure TfmMain.loadConf(path: String);
var
  f: File;
  s: String;
  parser: TJSONParser;
  jData: TJSONData;
  jObj: TJSONObject;
begin
  if (Assigned(FJSON)) then
     FJSON.Free;
  FJSON := nil;
  FConfigPath := '';
  if (Length(path) = 1) then begin
    //Supposed to create new config
    path := 'newConfig.json';
    AssignFile(f, path);
    Rewrite(f);
    CloseFile(f);
  end;

  if (not FileExists(path)) then begin
    ShowMessage('Cannot open file (404): ' + path);
    Exit;
  end;
  s := FileUtil.ReadFileToString(path);
  try
    parser := TJSONParser.Create(s, [joUTF8, joComments]);
    jData := parser.Parse;
    if (not Assigned(jData)) then begin
      jObj := TJSONObject.Create;
    end else begin
    if (not (jData.JSONType = jtObject)) then begin
      parser.Free;
      ShowMessage('Cannot parse file: Not a json object!');
      Exit;
    end;
    jObj := TJSONObject(jData);
    end;
  except
    on e: Exception do begin
      if (Assigned(parser)) then
         parser.Free;
      ShowMessage('Cannot parse file: ' + e.Message);
    end;
  end;
  FJSON := jObj;

  if (reloadFromJSON) then begin
    PageControl_main.ActivePage := TabSheet_config_generic;
    PageControl_mainChange(Self);
    TabSheet_config_generic.Enabled := True;
    FConfigPath := path;
  end else begin
      PageControl_main.ActivePage := TabSheet_selectFile;
      PageControl_mainChange(Self);
      TabSheet_config_generic.Enabled := False;
  end;
end;

function TfmMain.reloadFromJSON: Boolean;
var
  lTCP, jData, jData2: TJSONData;
  jObj, jObj2: TJSONObject;
  b, b2: Boolean;
  i, i2, i3, i4: Integer;
  s, s2: String;
begin
  try

    //Prepare TCP
    jData := FJSON.FindPath('tcp');
    if ((not Assigned(jData)) or (not (jData.JSONType = jtObject))) then begin
      lTCP := TJSONObject.Create;
      FJSON.Add('tcp', lTCP);
    end else begin
        lTCP := TJSONObject(jData);
    end;

    //Plain TCP
    b:= False;
    i := 0;
    jData := lTCP.FindPath('plain');
    if (not Assigned(jData) or (not (jData.JSONType = jtObject))) then begin
      jData := TJSONObject.Create;
    end;
    jObj := TJSONObject(jData);
    if (jObj.Get('enable', False)) then begin
       b := True;
       i := jObj.Get('port', 0);
    end;
    CheckBox_plainTcp.Checked := b;
    if (i<>0) then
       Label_portPlain.Caption := IntToStr(i)
    else
      Label_portPlain.Caption := '-';

    //Plain TCP
    b := False;
    i := 0;
    jData := lTCP.FindPath('ssl');
    if (not Assigned(jData) or (not (jData.JSONType = jtObject))) then begin
      jData := TJSONObject.Create;
    end;
    jObj := TJSONObject(jData);
    if (jObj.Get('enable', False)) then begin
       b := True;
       i := jObj.Get('port', 0);
    end;
    CheckBox_secTcp.Checked := b;
    if (i<>0) then
       Label_portSec.Caption := IntToStr(i)
    else
      Label_portSec.Caption := '-';

    //FT TCP
    b := False;
    i := 0;
    jData := lTCP.FindPath('ft');
    if (not Assigned(jData) or (not (jData.JSONType = jtObject))) then begin
      jData := TJSONObject.Create;
    end;
    jObj := TJSONObject(jData);
    if (jObj.Get('enable', False)) then begin
       b := True;
       i := jObj.Get('port', 0);
    end;
    CheckBox_ftTcp.Checked := b;
    if (i<>0) then
       Label_portFT.Caption := IntToStr(i)
    else
      Label_portFT.Caption := '-';

    //HW-DB-Cleanup
    b := False;
    i := 0;
    jData := FJSON.FindPath('cleanup');
    if ((not Assigned(jData)) or (not (jData.JSONType = jtObject))) then begin
       jData := TJSONObject.Create;
       FJSON.Add('cleanup', jData);
    end;
    jData := TJSONObject(jData).FindPath('hw_database');
    if ((not Assigned(jData)) or (not (jData.JSONType = jtObject))) then begin
       jData := TJSONObject.Create;

    end;
    jObj := TJSONObject(jData);
    if (jObj.Get('enable', False)) then begin
       b := True;
       i := jObj.Get('maxAgeDays', 0);
    end;
    CheckBox_cleanuphwDB.Checked := b;
    if (i<>0) then
       Label_maxhwDBAge.Caption := '>' + IntToStr(i) + 'd'
    else
      Label_maxhwDBAge.Caption := '-';

    //Automatic updates
    b := False;
    b2 := True;
    jData := FJSON.FindPath('update');
    if (Assigned(jData)) then begin
      if (jData.JSONType = jtObject) then begin
        jObj := TJSONObject(jData);
        if (jObj.Get('enable', False)) then
           b := True;
        if (not jObj.Get('upgrade', True)) then
           b2 := False;
        i := jObj.Get('interval', 0);
        s := jObj.Get('intervalTimeUnit', 'SECONDS');
      end;
    end;
    CheckBox_autoCheckUpdate.Checked := b;
    CheckBox_autoApplyUpdate.Checked := b2;
    if (i<>0) then begin
       Label_updateInterval.Caption := IntToStr(i);
       Label_updateIntervalTU.Caption := s;
    end else begin
      Label_updateInterval.Caption := '-';
      Label_updateIntervalTU.Caption := '-';
    end;

    //Groups+Users
    ListBox_groupsnusers.Clear;
    ListBox_groups.Clear;
    i3 := 0;
    i4 := 0;
    jData := FJSON.FindPath('groups');
    if ((not Assigned(jData)) or (not (jData.JSONType = jtObject))) then begin
      jData := TJSONObject.Create();
      FJSON.Add('groups', jData);
    end;
    jObj := TJSONObject(jData);
    for i:=0 to (jObj.Count-1) do begin
        s := jObj.Names[i];
        jData2 := jObj.FindPath(s + '.users');
        if ((not Assigned(jData2)) or (not (jData2.JSONType = jtObject))) then begin
           jData2 := TJSONObject.Create;
           TJSONObject(jData2).Add('users', TJSONObject.Create);
           jObj.Add(s, jData2);
        end;
        ListBox_groupsnusers.Items.Add(s);
        ListBox_groups.Items.Add(s);
        Inc(i3);
        jObj2 := TJSONObject(jData2);
        for i2:=0 to (jObj2.Count-1) do begin
            ListBox_groupsnusers.Items.Add('  ->' + jObj2.Names[i2]);
            Inc(i4);
        end;
    end;
    Label_groupCount.Caption := IntToStr(i3) + ' group(s)';
    Label_userCount.Caption := IntToStr(i4) + ' user(s)';

  except
  on e: Exception do begin
     ShowMessage('Cannot read config: ' + e.Message);
     Result := False;
  end;
end;
end;

