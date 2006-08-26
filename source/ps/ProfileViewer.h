/**
 * =========================================================================
 * File        : ProfileViewer.h
 * Project     : Pyrogenesis
 * Description : Viewing profiling information (timing and other statistics)
 *
 * @author Mark Thompson <mark@wildfiregames.com>
 * @author Nicolai Haehnle <nicolai@wildfiregames.com>
 * =========================================================================
 */

#ifndef PROFILE_VIEWER_INCLUDED
#define PROFILE_VIEWER_INCLUDED

#include "lib/input.h"
#include "ps/CStr.h"
#include "ps/Singleton.h"


/**
 * Struct ProfileColumn: Describes one column of an AbstractProfileTable.
 */
struct ProfileColumn
{
	/// Title of the column
	CStr title;
	
	/// Recommended width of the column, in pixels.
	uint width;
	
	ProfileColumn(const CStr& t, uint w) : title(t), width(w) { }
};


/**
 * Class AbstractProfileTable: Profile table data model.
 *
 * Clients that wish to display debug information in the profile viewer
 * have to implement this class and hook it into CProfileViewer.
 * 
 * Note that the profiling system is robust against deletion of
 * object instances in the sense that it will automatically remove
 * an AbstractProfileTable instance from its internal records when
 * you delete it.
 * Conversely, deleting an AbstractProfileTable instance is the responsibility
 * of its creator.
 */
class AbstractProfileTable
{
public:
	virtual ~AbstractProfileTable();
	
	/**
	 * GetName: Short descriptive name of this table (should be static).
	 *
	 * @return Descriptive name of this table.
	 */
	virtual CStr GetName() = 0;
	
	/**
	 * GetTitle: Longer, explanatory text (can be dynamic).
	 *
	 * @return Title for the table.
	 */
	virtual CStr GetTitle() = 0;
	
	
	/**
	 * GetNumberRows
	 *
	 * @return Number of rows in this table.
	 */
	virtual size_t GetNumberRows() = 0;
	
	/**
	 * GetColumnDescriptions
	 * 
	 * @return A vector describing all columns of the table.
	 */
	virtual const std::vector<ProfileColumn>& GetColumns() = 0;
	
	/**
	 * GetCellText
	 * 
	 * @param row Row index (the first row has index 0).
	 * @param col Column index (the first column has index 0).
	 * 
	 * @return Text to be displayed in the given cell.
	 */
	virtual CStr GetCellText(size_t row, size_t col) = 0;

	/**
	 * GetChild: Return a row's child table if the child is expandable.
	 *
	 * @param row Row index (the first row has index 0).
	 *
	 * @return Pointer to the child table if the given row has one.
	 * Otherwise, return 0.
	 */
	virtual AbstractProfileTable* GetChild(size_t row) = 0;
	
	/**
	 * IsHighlightRow
	 *
	 * @param row Row index (the first row has index 0).
	 *
	 * @return true if the row should be highlighted in a special color.
	 */
	virtual bool IsHighlightRow(size_t row) { UNUSED2(row); return false; }
};


struct CProfileViewerInternals;

/**
 * Class CProfileViewer: Manage and display profiling tables.
 */
class CProfileViewer : public Singleton<CProfileViewer>
{
	friend class AbstractProfileTable;
	
public:
	CProfileViewer();
	~CProfileViewer();

	/**
	 * RenderProfile: Render the profile display using OpenGL if the user
	 * has enabled it.
	 */
	void RenderProfile();
	
	/**
	 * Input: Filter and handle any input events that the profile display
	 * is interested in.
	 * 
	 * In particular, this function handles enable/disable of the profile
	 * display as well as navigating the information tree.
	 * 
	 * @param ev The incoming event.
	 * 
	 * @return IN_PASS or IN_HANDLED depending on whether the event relates
	 * to the profiling display.
	 */
	InReaction Input(const SDL_Event_* ev);
	
	/**
	 * AddRootTable: Add a new profile table as a root table (i.e. the
	 * tables that you cycle through via the profile hotkey).
	 *
	 * @note Tables added via this function are automatically removed from
	 * the list of root tables when they are deleted.
	 *
	 * @param table This table is added as a root table.
	 */
	void AddRootTable(AbstractProfileTable* table);

	/**
	 * InputThunk: Delegate to the singleton's Input() member function
	 * if the singleton has been initialized.
	 *
	 * This allows our input handler to be installed via in_add_handler
	 * like a normal, global function input handler.
	 */
	static InReaction InputThunk(const SDL_Event_* ev);

	/**
	 * SaveToFile: Saves the current profiler data (for all profile tables)
	 * to a file in the 'logs' directory.
	 */
	void SaveToFile();
	
private:
	CProfileViewerInternals* m;
};

#define g_ProfileViewer CProfileViewer::GetSingleton()

#endif
