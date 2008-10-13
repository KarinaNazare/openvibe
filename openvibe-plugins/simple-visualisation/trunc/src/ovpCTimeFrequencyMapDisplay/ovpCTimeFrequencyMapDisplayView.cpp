#include "ovpCTimeFrequencyMapDisplayView.h"

#include <iostream>
#include <sstream>

using namespace OpenViBE;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SimpleVisualisation;

using namespace OpenViBEToolkit;

using namespace std;

namespace OpenViBEPlugins
{
	namespace SimpleVisualisation
	{
		//Gtk widget callbacks
		void timeFrequencyMapToggleLeftRulerButtonCallback(::GtkWidget *widget, gpointer data);
		void timeFrequencyMapToggleBottomRulerButtonCallback(::GtkWidget *widget, gpointer data);
		gboolean timeFrequencyMapTimeScaleChangedCallback(::GtkSpinButton *widget,  gpointer data);
		void timeFrequencyMapChannelSelectButtonCallback(::GtkButton *button, gpointer data);
		void timeFrequencyMapChannelSelectDialogApplyButtonCallback(::GtkButton *button, gpointer data);
		gboolean timeFrequencyMapMinMaxAttenuationSpinButtonValueChangedCallback(::GtkSpinButton *widget,  gpointer data);
		void timeFrequencyMapMinDisplayedFrequencyChangedCallback(GtkSpinButton *widget, gpointer data);
		void timeFrequencyMapMaxDisplayedFrequencyChangedCallback(GtkSpinButton *widget, gpointer data);

		//
		//CTimeFrequencyMapDisplayView implementation
		//

		CTimeFrequencyMapDisplayView::CTimeFrequencyMapDisplayView(CSpectrumDatabase& rSpectrumDatabase,
			float64 f64MinDisplayedFrequency, float64 f64MaxDisplayedFrequency, float64 f64TimeScale) :
			m_rSpectrumDatabase(rSpectrumDatabase),
			m_pGladeInterface(NULL),
			m_pDisplayTable(NULL),
			m_f64Attenuation(0),
			m_f64MinDisplayedFrequency(f64MinDisplayedFrequency),
			m_f64MaxDisplayedFrequency(f64MaxDisplayedFrequency),
			m_pTimeRuler(NULL)
		{
			//load the glade interface
			m_pGladeInterface=glade_xml_new("../share/openvibe-plugins/simple-visualisation/openvibe-simple-visualisation-TimeFrequencyMapDisplay.glade", NULL, NULL);

			if(!m_pGladeInterface)
			{
				g_warning("Couldn't load the interface!");
				return;
			}

			glade_xml_signal_autoconnect(m_pGladeInterface);

			g_signal_connect(G_OBJECT(glade_xml_get_widget(m_pGladeInterface, "DisplayChannelSelectButton")), "clicked",       G_CALLBACK(timeFrequencyMapChannelSelectButtonCallback),     this);
			GtkSpinButton* l_pSpinButton = GTK_SPIN_BUTTON(glade_xml_get_widget(m_pGladeInterface, "DisplayTimeScale"));
			gtk_spin_button_set_value(l_pSpinButton, f64TimeScale);
			g_signal_connect(G_OBJECT(l_pSpinButton), "value-changed", G_CALLBACK(timeFrequencyMapTimeScaleChangedCallback),  this);
			g_signal_connect(G_OBJECT(glade_xml_get_widget(m_pGladeInterface, "DisplayMinMaxAttenuationSpin")), "value-changed", G_CALLBACK(timeFrequencyMapMinMaxAttenuationSpinButtonValueChangedCallback),  this);

			//channel select dialog's signals
			g_signal_connect(G_OBJECT(glade_xml_get_widget(m_pGladeInterface, "DisplayChannelSelectApplyButton")), "clicked", G_CALLBACK(timeFrequencyMapChannelSelectDialogApplyButtonCallback), this);

			//connect the cancel button to the dialog's hide command
			g_signal_connect_swapped(G_OBJECT(glade_xml_get_widget(m_pGladeInterface, "DisplayChannelSelectCancelButton")),
				"clicked",
				G_CALLBACK(gtk_widget_hide),
				G_OBJECT(glade_xml_get_widget(m_pGladeInterface, "DisplayChannelSelectDialog")));

			//hides the dialog if the user tries to close it
			g_signal_connect (G_OBJECT(glade_xml_get_widget(m_pGladeInterface, "DisplayChannelSelectDialog")),
				"delete_event", G_CALLBACK(gtk_widget_hide), NULL);

			//does nothing on the main window if the user tries to close it
			g_signal_connect (G_OBJECT(glade_xml_get_widget(m_pGladeInterface, "DisplayMainWindow")),
				"delete_event", G_CALLBACK(gtk_widget_hide), NULL);

			// gets attenuation
			m_f64Attenuation=(gtk_spin_button_get_value(GTK_SPIN_BUTTON(glade_xml_get_widget(m_pGladeInterface, "DisplayMinMaxAttenuationSpin"))));
		}

		CTimeFrequencyMapDisplayView::~CTimeFrequencyMapDisplayView()
		{
			//destroy channel widgets
			for(uint32 i=0 ; i<m_vChannelDisplays.size() ; i++)
			{
				delete m_vChannelDisplays[i];
			}

			//unref the xml file as it's not needed anymore
			g_object_unref(G_OBJECT(m_pGladeInterface));
			m_pGladeInterface=NULL;
		}

		boolean CTimeFrequencyMapDisplayView::init()
		{
			//tell database about time scale to display
			m_rSpectrumDatabase.setTimeScale(
				gtk_spin_button_get_value(
					GTK_SPIN_BUTTON(
						glade_xml_get_widget(
							m_pGladeInterface, "DisplayTimeScale"))));

			//tell database about frequency range to display
			//TODO
			//m_rSpectrumDatabase.setDisplayedFrequencyRange(m_f64MinDisplayedFrequency, m_f64MaxDisplayedFrequency);

			//size group for the channel labels and the empty widget in the bottom bar
			//(useful to position the bottom time ruler correctly)
			::GtkSizeGroup * l_pSizeGroup = gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);

			//retrieve channel count
			uint32 l_ui32ChannelCount = m_rSpectrumDatabase.getChannelCount();

			//create array of channel displays
			m_vChannelDisplays.resize(l_ui32ChannelCount);
			m_vChannelLabels.resize(l_ui32ChannelCount);

			//get pointer to channels table
			m_pDisplayTable = glade_xml_get_widget(m_pGladeInterface, "DisplayMainTable");

			//resize this table according to the number of channels
			gtk_table_resize(GTK_TABLE(m_pDisplayTable), (l_ui32ChannelCount)*2, 3);

			//sets a minimum size for the table (needed to scroll)
			gtk_widget_set_size_request(m_pDisplayTable, 20, (l_ui32ChannelCount)*20);

			//add a vertical separator
			::GtkWidget* l_pSeparator = gtk_vseparator_new();
			gtk_table_attach(GTK_TABLE(m_pDisplayTable), l_pSeparator,
				1, 2, 0, (l_ui32ChannelCount)*2, GTK_SHRINK, static_cast< ::GtkAttachOptions >(GTK_EXPAND | GTK_FILL), 0, 0);
			gtk_widget_show(l_pSeparator);

			//create as many widgets as there are channels and add them to table
			CString l_oChannelLabel;
			stringstream l_oLabelString;
			::GtkWidget * l_pChannelSelectList = glade_xml_get_widget(m_pGladeInterface, "DisplayChannelSelectList");

			for(uint32 i=0 ; i<l_ui32ChannelCount ; i++)
			{
				m_rSpectrumDatabase.getChannelLabel(i, l_oChannelLabel);

				//adds a label with the channel's name
				if(l_oChannelLabel.toASCIIString() == "")
				{
					//If no name has been set, just keep the channel index
					l_oLabelString<<"Channel "<<i;
				}
				else
				{	//else keep the index plus the name
					l_oLabelString<<i<<" : "<<l_oChannelLabel;
				}
				::GtkWidget * l_pLabel =  gtk_label_new(l_oLabelString.str().c_str());

				m_vChannelLabels[i] = l_pLabel;

				gtk_table_attach(GTK_TABLE(m_pDisplayTable),l_pLabel,
					0, 1, i*2, (i*2)+1,
					GTK_FILL, GTK_SHRINK,
					0, 0);

				gtk_widget_show(l_pLabel);

				//add label to size group
				gtk_size_group_add_widget(l_pSizeGroup, l_pLabel);

				//create channel display widget
				m_vChannelDisplays[i] = new CTimeFrequencyMapChannelDisplay(m_rSpectrumDatabase, i, m_f64Attenuation,
					m_f64MinDisplayedFrequency, m_f64MaxDisplayedFrequency);

				//add it to table
				gtk_table_attach(GTK_TABLE(m_pDisplayTable), m_vChannelDisplays[i]->getTopWidget(),
				2, 3, (i*2), (i*2)+1,
				static_cast< ::GtkAttachOptions >(GTK_EXPAND | GTK_FILL),
				static_cast< ::GtkAttachOptions >(GTK_EXPAND | GTK_FILL), 0, 0);

				gtk_widget_show_all(m_vChannelDisplays[i]->getTopWidget());

				//add a horizontal separator under it
				l_pSeparator = gtk_hseparator_new();
				gtk_table_attach(GTK_TABLE(m_pDisplayTable), l_pSeparator,
				0, 3, (i*2)+1, (i*2)+2,	static_cast< ::GtkAttachOptions >(GTK_EXPAND | GTK_FILL), GTK_SHRINK, 0, 0);
				gtk_widget_show(l_pSeparator);

				//add a checkbox in the channel select window
				::GtkWidget * l_pChannelCheckButton = gtk_check_button_new_with_label(l_oLabelString.str().c_str());
				m_vChannelsCheckButtons.push_back(l_pChannelCheckButton);

				gtk_box_pack_start_defaults(GTK_BOX(l_pChannelSelectList), l_pChannelCheckButton);

				l_oLabelString.str("");

				//a channel is selected by default
				m_vSelectedChannels.push_back(i);
			}

			//bottom ruler
			//------------
			m_pTimeRuler = new CTimeRuler(m_rSpectrumDatabase);

			//add empty label to size group (it will request the same height than the channel labels)
			gtk_size_group_add_widget(l_pSizeGroup, glade_xml_get_widget(m_pGladeInterface, "DisplayBottomBarEmptyLabel1"));

			//add bottom ruler to bottom bar (after empty label)
			gtk_box_pack_start(GTK_BOX(glade_xml_get_widget(m_pGladeInterface, "DisplayBottomBar")),
				m_pTimeRuler->getWidget(),
				false, false, 0);

			// tell ruler it has to resize along with channel displays
			if(m_vChannelDisplays.size() != 0)
			{
				m_pTimeRuler->linkWidthToWidget(m_vChannelDisplays[0]->getMapWidget());
			}

			//displayed frequencies spin buttons
			//----------------------------------

			//retrieve input frequency range
			float64 l_f64MinFrequency = m_rSpectrumDatabase.getFrequencyBandStart(0);
			float64 l_f64MaxFrequency = m_rSpectrumDatabase.getFrequencyBandStop(m_rSpectrumDatabase.getFrequencyBandCount()-1);

			//ensure displayed frequencies lie within that range
			if(m_f64MinDisplayedFrequency < l_f64MinFrequency)
			{
				m_f64MinDisplayedFrequency = l_f64MinFrequency;
			}
			else if(m_f64MinDisplayedFrequency > l_f64MaxFrequency)
			{
				m_f64MinDisplayedFrequency = l_f64MaxFrequency;
			}

			if(m_f64MaxDisplayedFrequency < l_f64MinFrequency)
			{
				m_f64MaxDisplayedFrequency = l_f64MinFrequency;
			}
			else if(m_f64MaxDisplayedFrequency > l_f64MaxFrequency)
			{
				m_f64MaxDisplayedFrequency = l_f64MaxFrequency;
			}

			//send displayed frequency range to database so that min/max computations are restricted to it
			//FIXME
			/*m_pBufferDatabase->setMinDisplayedFrequency(m_f64MinDisplayedFrequency);
			m_pBufferDatabase->setMaxDisplayedFrequency(m_f64MaxDisplayedFrequency);*/

			//initialize spin buttons and connect callbacks
			GtkSpinButton* l_pMinFrequencySpinButton = GTK_SPIN_BUTTON(glade_xml_get_widget(m_pGladeInterface, "MinFrequency"));
			if(l_pMinFrequencySpinButton != NULL)
			{
				gtk_spin_button_set_range(l_pMinFrequencySpinButton, 0, l_f64MaxFrequency);
				gtk_spin_button_set_value(l_pMinFrequencySpinButton, m_f64MinDisplayedFrequency);
				g_signal_connect(G_OBJECT(l_pMinFrequencySpinButton), "value-changed", G_CALLBACK(timeFrequencyMapMinDisplayedFrequencyChangedCallback), this);
			}

			GtkSpinButton* l_pMaxFrequencySpinButton = GTK_SPIN_BUTTON(glade_xml_get_widget(m_pGladeInterface, "MaxFrequency"));
			if(l_pMaxFrequencySpinButton != NULL)
			{
				gtk_spin_button_set_range(l_pMaxFrequencySpinButton, 0, l_f64MaxFrequency);
				gtk_spin_button_set_value(l_pMaxFrequencySpinButton, m_f64MaxDisplayedFrequency);
				g_signal_connect(G_OBJECT(l_pMaxFrequencySpinButton), "value-changed", G_CALLBACK(timeFrequencyMapMaxDisplayedFrequencyChangedCallback), this);
			}

			for(uint32 i=0; i<m_vChannelDisplays.size(); i++)
			{
				m_vChannelDisplays[i]->setDisplayedFrequencyRange(m_f64MinDisplayedFrequency, m_f64MaxDisplayedFrequency);
			}

			//set up initial states
			//---------------------

			//activate buttons by default
			activateToolbarButtons(true);

			//display left rulers
			boolean l_bDefaultLeftRulersToggleState = true;
			toggleLeftRulers(l_bDefaultLeftRulersToggleState);
			gtk_toggle_tool_button_set_active(GTK_TOGGLE_TOOL_BUTTON(glade_xml_get_widget(m_pGladeInterface, "DisplayToggleLeftRuler")), l_bDefaultLeftRulersToggleState ? TRUE : FALSE);
			g_signal_connect(G_OBJECT(glade_xml_get_widget(m_pGladeInterface, "DisplayToggleLeftRuler")),	"toggled", G_CALLBACK(timeFrequencyMapToggleLeftRulerButtonCallback), this);

			//display bottom ruler
			boolean l_bDefaultBottomRulerToggleState = true;
			toggleBottomRulers(l_bDefaultBottomRulerToggleState);

			gtk_toggle_tool_button_set_active(GTK_TOGGLE_TOOL_BUTTON(glade_xml_get_widget(m_pGladeInterface, "DisplayToggleBottomRuler")), l_bDefaultBottomRulerToggleState ? TRUE : FALSE);
			g_signal_connect(G_OBJECT(glade_xml_get_widget(m_pGladeInterface, "DisplayToggleBottomRuler")),	"toggled", G_CALLBACK(timeFrequencyMapToggleBottomRulerButtonCallback), this);

			return true;
		}

		boolean CTimeFrequencyMapDisplayView::redraw()
		{
			for(size_t i=0 ; i<m_vChannelDisplays.size() ; i++)
			{
				m_vChannelDisplays[i]->update();
			}

			//nothing to redraw if the table isn't visible
			if(m_pDisplayTable && GTK_WIDGET_VISIBLE(m_pDisplayTable))
			{
				//if(m_pBufferDatabase->getChannelCount() > 0)
				{
					//The ruler needs to be redrawn
					gdk_window_invalidate_rect(GTK_WIDGET(m_pTimeRuler->getWidget())->window,
							NULL,
							true);

					//The channels table needs to be redrawn
					gdk_window_invalidate_rect(GTK_WIDGET(m_pDisplayTable)->window,
							NULL,
							true);
				}
			}

			return true;
		}

		void CTimeFrequencyMapDisplayView::getWidgets(::GtkWidget*& pWidget, ::GtkWidget*& pToolbarWidget)
		{
			pWidget=glade_xml_get_widget(m_pGladeInterface, "FrequencySpectrumDisplayScrolledWindow");
			pToolbarWidget=glade_xml_get_widget(m_pGladeInterface, "Toolbar");
		}

		void CTimeFrequencyMapDisplayView::toggleLeftRulers(boolean bActive)
		{
			for(size_t i=0 ; i<m_vChannelDisplays.size() ; i++)
			{
				m_vChannelDisplays[i]->toggleLeftRuler(bActive);
			}
		}

		void CTimeFrequencyMapDisplayView::toggleBottomRulers(boolean bActive)
		{
			m_pTimeRuler->toggle(bActive);
		}

		void CTimeFrequencyMapDisplayView::hideChannel(uint32 ui32ChannelIndex)
		{
			m_vChannelDisplays[ui32ChannelIndex]->toggle(false);
			gtk_widget_hide( m_vChannelLabels[ui32ChannelIndex] );
		}

		void CTimeFrequencyMapDisplayView::showChannel(uint32 ui32ChannelIndex)
		{
			m_vChannelDisplays[ui32ChannelIndex]->toggle(true);
			gtk_widget_show( m_vChannelLabels[ui32ChannelIndex] );
		}

		void CTimeFrequencyMapDisplayView::updateMainTableStatus()
		{
			//if nothing has been selected
			if(m_vSelectedChannels.empty())
			{
				//hide the whole table
				gtk_widget_hide(GTK_WIDGET(m_pDisplayTable));
			}
			else
			{
				if(!GTK_WIDGET_VISIBLE(GTK_WIDGET(m_pDisplayTable)))
				{
					//if there were no selected channels before, but now there are, show the table again
					gtk_widget_show(GTK_WIDGET(m_pDisplayTable));
				}
			}
		}

		void CTimeFrequencyMapDisplayView::activateToolbarButtons(boolean bActive)
		{
			// gtk_widget_set_sensitive(glade_xml_get_widget(m_pGladeInterface, "SignalDisplayToggleLeftRuler"), bActive);
			gtk_widget_set_sensitive(glade_xml_get_widget(m_pGladeInterface, "DisplayToggleBottomRuler"), bActive);
			gtk_widget_set_sensitive(glade_xml_get_widget(m_pGladeInterface, "DisplayTimeScaleItem"), bActive);
			gtk_widget_set_sensitive(glade_xml_get_widget(m_pGladeInterface, "DisplayChannelSelectButton"), bActive);
		}

		void CTimeFrequencyMapDisplayView::setMinMaxAttenuation(float64 f64Attenuation)
		{
			m_f64Attenuation=f64Attenuation;

			for(uint32 i=0 ; i<m_vChannelDisplays.size() ; i++)
			{
				m_vChannelDisplays[i]->setMinMaxAttenuation(m_f64Attenuation);
			}
		}

		void CTimeFrequencyMapDisplayView::minDisplayedFrequencyChangedCB(GtkWidget* pWidget)
		{
			m_f64MinDisplayedFrequency = gtk_spin_button_get_value(GTK_SPIN_BUTTON(pWidget));

			//TODO (if min/max computation should be restricted to this range)
			//m_rSpectrumDatabase.setDisplayedFrequencyRange(m_f64MinDisplayedFrequency, m_f64MaxDisplayedFrequency);

			for(uint32 i=0 ; i<m_vChannelDisplays.size() ; i++)
			{
				m_vChannelDisplays[i]->setDisplayedFrequencyRange(m_f64MinDisplayedFrequency, m_f64MaxDisplayedFrequency);
			}

			//redraw left ruler
			//TODO
			//gdk_window_invalidate_rect(m_pLeftRuler->window, NULL, true);
		}

		void CTimeFrequencyMapDisplayView::maxDisplayedFrequencyChangedCB(GtkWidget* pWidget)
		{
			m_f64MaxDisplayedFrequency = gtk_spin_button_get_value(GTK_SPIN_BUTTON(pWidget));

			//TODO (if min/max computation should be restricted to this range)
			//m_rSpectrumDatabase.setDisplayedFrequencyRange(m_f64MinDisplayedFrequency, m_f64MaxDisplayedFrequency);

			for(uint32 i=0 ; i<m_vChannelDisplays.size() ; i++)
			{
				m_vChannelDisplays[i]->setDisplayedFrequencyRange(m_f64MinDisplayedFrequency, m_f64MaxDisplayedFrequency);
			}

			//redraw left ruler
			//TODO
			//gdk_window_invalidate_rect(m_pLeftRuler->window, NULL, true);
		}

		void CTimeFrequencyMapDisplayView::timeScaleChangedCB(GtkSpinButton* pWidget)
		{
			boolean l_bNumberOfDisplayedBufferChanged = m_rSpectrumDatabase.setTimeScale(gtk_spin_button_get_value(pWidget));

			if(l_bNumberOfDisplayedBufferChanged)
			{
				redraw();
			}
		}

		void CTimeFrequencyMapDisplayView::channelSelectionDialogOpenedCB(GtkButton* pButton)
		{
			::GtkWidget * l_pChannelSelectDialog = glade_xml_get_widget(m_pGladeInterface, "DisplayChannelSelectDialog");

			//reset all checkbuttons
			for(size_t i=0 ; i<m_vChannelsCheckButtons.size() ; i++)
			{
				gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_vChannelsCheckButtons[i]), false);
			}

			//sets only channels in the list
			for(size_t i=0 ; i<m_vSelectedChannels.size() ; i++)
			{
				gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_vChannelsCheckButtons[(size_t)m_vSelectedChannels[i]]), true);
			}

			//finally, show the information dialog
			gtk_widget_show_all(l_pChannelSelectDialog);
		}

		void CTimeFrequencyMapDisplayView::channelSelectionChangedCB(GtkButton* pButton)
		{
			//clears the list of selected channels
			m_vSelectedChannels.clear();

			for(unsigned int i=0 ; i<m_vChannelsCheckButtons.size() ; i++)
			{
				if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_vChannelsCheckButtons[i])))
				{
					//if a button is checked, add the corresponding channel to the list of selected ones
					m_vSelectedChannels.push_back(i);
					//show the channel's display
					showChannel(i);
				}
				else
				{
					//this channel is not selected, hides it
					hideChannel(i);
				}
			}

			updateMainTableStatus();

			//hides the channel selection dialog
			gtk_widget_hide(glade_xml_get_widget(m_pGladeInterface, "DisplayChannelSelectDialog"));
		}

		//
		//CALLBACKS
		//

		void timeFrequencyMapToggleLeftRulerButtonCallback(::GtkWidget *widget, gpointer data)
		{
			CTimeFrequencyMapDisplayView* l_pView = reinterpret_cast<CTimeFrequencyMapDisplayView*>(data);
			l_pView->toggleLeftRulers(gtk_toggle_tool_button_get_active(GTK_TOGGLE_TOOL_BUTTON(widget))?true:false);
		}

		void timeFrequencyMapToggleBottomRulerButtonCallback(::GtkWidget *widget, gpointer data)
		{
			CTimeFrequencyMapDisplayView* l_pView = reinterpret_cast<CTimeFrequencyMapDisplayView*>(data);
			l_pView->toggleBottomRulers(gtk_toggle_tool_button_get_active(GTK_TOGGLE_TOOL_BUTTON(widget))?true:false);
		}

		gboolean timeFrequencyMapTimeScaleChangedCallback(::GtkSpinButton *widget,  gpointer data)
		{
			CTimeFrequencyMapDisplayView* l_pView = reinterpret_cast<CTimeFrequencyMapDisplayView*>(data);
			l_pView->timeScaleChangedCB(widget);
			return FALSE;
		}

		//called when the channel select button is pressed (opens the channel selection dialog)
		void timeFrequencyMapChannelSelectButtonCallback(::GtkButton *button, gpointer data)
		{
			CTimeFrequencyMapDisplayView* l_pView = reinterpret_cast<CTimeFrequencyMapDisplayView*>(data);
			l_pView->channelSelectionDialogOpenedCB(button);
		}

		//Called when the user press the apply button of the channel selection dialog
		void timeFrequencyMapChannelSelectDialogApplyButtonCallback(::GtkButton *button, gpointer data)
		{
			CTimeFrequencyMapDisplayView* l_pView = reinterpret_cast<CTimeFrequencyMapDisplayView*>(data);
			l_pView->channelSelectionChangedCB(button);
		}

		gboolean timeFrequencyMapMinMaxAttenuationSpinButtonValueChangedCallback(::GtkSpinButton *widget,  gpointer data)
		{
			reinterpret_cast<CTimeFrequencyMapDisplayView*>(data)->setMinMaxAttenuation(static_cast<float64>(gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget))));
			return FALSE;
		}

		void timeFrequencyMapMinDisplayedFrequencyChangedCallback(GtkSpinButton *widget, gpointer data)
		{
			CTimeFrequencyMapDisplayView* l_pTimeFrequencyMapDisplayView = reinterpret_cast<CTimeFrequencyMapDisplayView*>(data);
			l_pTimeFrequencyMapDisplayView->minDisplayedFrequencyChangedCB(GTK_WIDGET(widget));
		}

		void timeFrequencyMapMaxDisplayedFrequencyChangedCallback(GtkSpinButton *widget, gpointer data)
		{
			CTimeFrequencyMapDisplayView* l_pTimeFrequencyMapDisplayView = reinterpret_cast<CTimeFrequencyMapDisplayView*>(data);
			l_pTimeFrequencyMapDisplayView->maxDisplayedFrequencyChangedCB(GTK_WIDGET(widget));
		}
	}
}
