/' Zajisteni zakladni kompatibility IFJ17->FreeBASIC '/

Function Length(s As String) As Integer
  Return Len(s)
End Function

Function SubStr(s as String, i As Integer, n As Integer) As String
  Return Mid(s, i, n)
End Function

/' Zde bude nasledovat program jazyka IFJ17 '/
