IF EXIST game (
echo "game/ folder exists"
@RD /S /Q "game"
)
mkdir game
xcopy /s /y .\dll\Release\x64 .\game
echo d | xcopy /s /y .\game_client\audio .\game\audio
echo d | xcopy /s /y .\game_client\font .\game\font
echo d | xcopy /s /y .\game_client\model .\game\model
echo d | xcopy /s /y .\game_client\texture .\game\texture
xcopy .\game_client\*.glsl .\game
xcopy .\game_client\*.ini .\game
xcopy .\game_server\*.ini .\game
xcopy .\x64\Release\*.exe .\game