#ifndef	ALIENHACK_CONSOLE_DRAW_HPP
#define	ALIENHACK_CONSOLE_DRAW_HPP


#include "DBKeyValue.hpp"
#include "ActionEngine/ActionEngine.hpp" //for GameTimeCoordinate


namespace RL_shared
{
	class Console;
	class UserMessageLog;
	class UserMessageWindow;
}

namespace AlienHack
{

	class AHGameModel;

	namespace cursor_type
	{
		enum Cursor
		{
			None = 0,
			Target, 
			Look
		};
	}

	const int PING_RANGE_SQUARED = 16*16;

	void drawWorld(
			RL_shared::Console&, const AHGameModel&, 
			RL_shared::DBKeyValue focus_zone, 
			int focus_x, int focus_z, 
			int vis_focus_x, int vis_focus_z, 
			int x_low, int x_high, 
			int y_low, int y_high, 
			cursor_type::Cursor cursor, 
			int cursor_offs_x, int cursor_offs_z, 
			bool no_visibility, 
			bool draw_pings
			);


	const int WORLD_VIEW_X_LOW = 2;
	const int WORLD_VIEW_X_HI = 41;
	const int WORLD_VIEW_Y_LOW = 1;
	const int WORLD_VIEW_Y_HI = 33;

	void drawFrame( RL_shared::Console&, bool redframe );
	void drawOuterFrame( RL_shared::Console&, bool redframe );
	void drawInnerFrame( RL_shared::Console&, bool redframe );
	void drawZoneName( RL_shared::Console&, const AHGameModel&, 
			RL_shared::DBKeyValue focus_zone, bool red );

	void writeSelectedItems(
			RL_shared::Console&, const AHGameModel&, 
			RL_shared::DBKeyValue focus_zone, 
			int focus_x, int focus_z, 
			int vis_focus_x, int vis_focus_z, 
			bool inred, 
			bool show_pings, 
			bool show_items
			);

	const int MSGS_X_LOW = 45;
	const int MSGS_X_HI = 77;
	const int MSGS_Y_LOW = 21;
	const int MSGS_Y_HI = 38;

	//void drawMessageWindow( 
	//	RL_shared::Console& window, 
	//	const RL_shared::UserMessageLog& msg_log, 
	//	const RL_shared::UserMessageWindow& msg_window, 
	//	int window_x_low, int window_y_low, int window_x_high, int window_y_high, 
	//	bool highlight_last
	//	);

	const int HUD_X_LOW = 45;
	const int HUD_X_HI = 77;
	const int HUD_Y_LOW = 1;
	const int HUD_Y_HI = 19;

	void drawHUD( 
		RL_shared::Console&, const AHGameModel&, 
		int x_low, int y_low, 
		int x_high, int y_high, 
		RL_shared::GameTimeCoordinate elapsed_time
		);
}


#endif
