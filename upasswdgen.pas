unit uPassWdGen;

{$mode objfpc}{$H+}

interface

uses
  Classes, SysUtils;

const

  ALPHA: Array[0..25] of Char =
    ('a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z');
  NUMS: Array[0..9] of Char =
    ('0','1','2','3','4','5','6','7','8','9');
  SYMBOLS: Array[0..18] of Char =
    (',','.','-','_','!','"','#','@','?','[',']','\','^','{','}','|','<','>','~');

type
  GenPasswdOption = (
    gpwdoAlphaUPPER,
    gpwdoAlphaLOWER,
    gpwdoNumeric,
    gpwdoSymbols
  );
  GenPasswdOptions = Set of GenPasswdOption;

function genPasswd(len: Integer; options: GenPasswdOptions): String;

implementation

function genPasswd(len: Integer; options: GenPasswdOptions): String;
var
  source: Array of Char;
  l, i: Integer;
begin
  Result := '';

  //Fill source array
  SetLength(source, 0);
  if (gpwdoAlphaLOWER in options) then begin
    l := Length(source);
    SetLength(source, l + Length(ALPHA));
    for i:=0 to Length(ALPHA)-1 do begin
      source[l+i] := ALPHA[i];
    end;
  end;
  if (gpwdoAlphaUPPER in options) then begin
    l := Length(source);
    SetLength(source, l + Length(ALPHA));
    for i:=0 to Length(ALPHA)-1 do begin
      source[l+i] := UpperCase(ALPHA[i])[1];
    end;
  end;
  if (gpwdoNumeric in options) then begin
    l := Length(source);
    SetLength(source, l + Length(NUMS));
    for i:=0 to Length(NUMS)-1 do begin
      source[l+i] := NUMS[i];
    end;
  end;
  if (gpwdoSymbols in options) then begin
    l := Length(source);
    SetLength(source, l + Length(SYMBOLS));
    for i:=0 to Length(SYMBOLS)-1 do begin
      source[l+i] := SYMBOLS[i];
    end;
  end;
  if (Length(source) = 0) then
     source := ALPHA;
  l := Length(source);

  for i:=0 to len-1 do begin
    Result := Result + source[Random(l) - 1];
  end;
end;

end.

