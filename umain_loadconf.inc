{THIS FILE CONTAINS THE CODE THAT'S SUPPOSED TO LOAD THE CONFIG}

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
end;

end.
