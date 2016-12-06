unit uConfSearch;

{$mode objfpc}{$H+}

interface

uses
  Classes, SysUtils, FileUtil;

type

  TConfSearch = class;
  POnConfSearchDone = procedure (sender: TConfSearch) of Object;

  { TConfSearch }

  TConfSearch = class(TThread)
    protected
      procedure Execute; override;
    private
      onDone: POnConfSearchDone;
      FResults: TStringList;
    public
      //Setup
      constructor Create;
      destructor Destroy; override;
      procedure SetOnDone(onDoneProc: POnConfSearchDone);
      function getResults: TStringList;
    private
      procedure sendDone;
  end;

implementation

{ TConfSearch }

procedure TConfSearch.Execute;
var
  tempList: TStringList;
  I: Integer;
begin
  tempList := FindAllFiles(GetCurrentDir, 'config.json;*.conf', True);
  for I:=0 to (tempList.Count - 1) do begin
    FResults.Add(tempList.Strings[I]);
  end;
  Sleep(1000);
  Synchronize(@sendDone);
end;

constructor TConfSearch.Create;
begin
  inherited Create(True);
  FResults := TStringList.Create;
end;

destructor TConfSearch.Destroy;
begin
  FResults.Free;
  inherited Destroy;
end;

procedure TConfSearch.SetOnDone(onDoneProc: POnConfSearchDone);
begin
  onDone := onDoneProc;
end;

function TConfSearch.getResults: TStringList;
begin
  Result := FResults;
end;

procedure TConfSearch.sendDone;
begin
  if (Assigned(onDone)) then
     onDone(Self);
end;

end.

