/**
 * \file qmdiclient.cpp
 * \brief Implementation of the qmdi client class
 * \author Diego Iastrubni (diegoiast@gmail.com)
 * License LGPL 2 or 3
 * \see qmdiClient
 */

#include "qmdiclient.h"
#include "qmdihost.h"
#include "qmdiserver.h"
#include <QObject>
#include <QWidget>

/**
 * \class qmdiClient
 * \brief Interface for menus and toolbars
 *
 * If you want to use qmdilib, every one of your widgets must inherit
 * qmdiClient and define their own menus using this class. Any widget
 * which will be inserted into a qmdiServer and does not inherit qmdiClient
 * will not show special menus and toolbars.
 *
 * On the documentation of qmdilib, any widget inserted into a qmdiServer
 * that inherits qmdiClient will be called \b MDI \b client. In the following
 * example the class \b newclient is a \b MDI \b client.
 *
 * \code
 * class newclient: public QTextEdit, public qmdiClient
 * {
 *     ...
 * };
 * \endcode
 *
 * Defining menus and toolbars is easy:
 *
 * \code
 * newclient::newclient()
 * {
 * 	// some code ...
 *
 *	menus["&File"]->addAction( actionNew );
 *	menus["&File"]->addSeparator();
 *	menus["&File"]->addAction( actionQuit );
 *
 * 	toolbars["General"]->addAction( actionNew );
 * }
 *
 * \endcode
 *
 * You will have also to insert this MDI client into an MDI server
 * (for example qmdiTabWidget). The menus and toolbars will be merged
 * automatically for you every time you widget is selected on the qmdiServer.
 *
 * \see qmdiActionGroupList
 * \see qmdiTabWidget
 */

/**
 * \brief default constructor
 * \param newName the name of the client.
 *
 * Builds a new client assigning it an mdi client name of newName and assigning
 * a nullptr mdiServer.
 *
 */
qmdiClient::qmdiClient(const QString &newName) {
    mdiServer = nullptr;
    mdiClientName = newName;
}

/**
 * \brief default destructor
 *
 * Destructs the mdi client, and if an mdiServer is defined it will ask the
 * server to delete later this client.
 *
 * It will also set the mdi server to nullptr.
 */
qmdiClient::~qmdiClient() {
    if (mdiServer != nullptr) {
        mdiServer->deleteClient(this);
    }

    mdiServer = nullptr;
}

/**
 * \function qmdiClient::nClientClosed(qmdiClient *client)
 * \brief callback called when a client has been removed
 *
 * When a client is finally removed from an mdi host, this function
 * is called. The goal is to notify the host about it (for example
 * to store the list of closed documents).
 *
 * By default does nothing, implement in subclasses to add the needed
 * functionality.
 */

/**
 * \var qmdiClient::mdiClientName
 * \brief The name of the MDI client
 *
 * This property defines the name of the MDI client. The \b mdiClientName
 * is used by qmdiTabWidget for setting the tab name, when inserting
 * the widget into the tab widget.
 *
 * \copydoc read_only_after_constructor
 */

/**
 * \var qmdiClient::menus
 * \brief The list of menus defined by this MDI client
 *
 * The list of menus this MDI client defines, and will be merged
 * each time the MDI client is showed on screen.
 *
 * The syntax for defining menus is:
 *
 * \code
 * menus->addAction( actionNew );
 * menus->addAction( actionOpen );
 * menus->addAction( actionSave );
 * menus->addSeparator();
 * menus->addAction( actionQuit );
 * \endcode
 *
 * \see qmdiClient::toolbars
 */

/**
 * \var qmdiClient::toolbars
 * \brief The list of toolbars defined by this MDI client
 *
 * The list of toolbars this MDI client defines, and will be merged
 * each time the MDI client is showed on screen.
 *
 * The syntax for defining toolbars is:
 *
 * \code
 * toolbars->addAction( actionNew );
 * toolbars->addSeparator();
 * toolbars->addAction( actionOpen );
 * toolbars->addAction( actionSave );
 * \endcode
 *
 * \see qmdiClient::menus
 */

/**
 * \var qmdiClient::mdiServer
 * \brief A pointer to the MDI server in which this client is inserted.
 *
 * When the MDI client is inserted into a MDI server, this property
 * is set to point to the corresponding MDI server. This property is
 * read only, and you should not assign value to it.
 *
 * You can use this property to insert new clients into the server,
 * from this client.
 *
 * \copydoc read_only_property
 * \see qmdiServer
 */

/**
 * \brief close the MDI client
 * \return true if the widget is closed after this call
 *
 * If you want to delete MDI clients from your MDI server, sometimes
 * just deleting this client is not the smart move. For example if the
 * client contains some user editable document, the user should be asked
 * to save his changes, and maybe even abort.
 *
 * By default this function does:
 *  - calls canCloseClient to find out if it's OK to closeClient()
 *  - if the function returns true, it will try to cast the MDI client to a
 *    QObject, calls deleteLater() and returns true and the object
 *    will be deleted afterwards.
 *  - if cast failed or canCloseClient() returned false, a negative value
 *    is returned, and the object is not destructed.
 *  - The mdi host is notified that this client has been finally closed
 *    by calling onClientClosed()
 *
 * This means that for read only clients you can leave the default. On R/W
 * clients which derive QObject, you will have to override canCloseClient().
 * If your derived class does not derive QObject, you will need to overide this
 * function as well.
 *
 * \see canCloseClient()
 * \see QObject::deleteLater()
 * \see qmdiHost::onClientClosed()
 */
bool qmdiClient::closeClient() {
    if (!canCloseClient()) {
        return false;
    }
    this->mdiServer->mdiHost->unmergeClient(this);
    this->mdiServer->mdiHost->onClientClosed(this);
    if (auto o = dynamic_cast<QObject *>(this)) {
        if (auto w = qobject_cast<QWidget *>(o)) {
            w->hide();
        }
        o->deleteLater();
    }
    return true;
}

/**
 * \brief check if the MDI client is valid for closing
 * \return true is it is OK to destruct the MDI client
 *
 * This function should return true if the MDI client is safe for
 * removal from the MDI client. Clients are valid for removal usually
 * if no changes have been made, or the user accepted to save then.
 *
 * You can define your own logic by re-implementing this function
 * in your derived classes. In rare cases, closeClient() should be
 * re-implemented as well.
 *
 * \see closeClient()
 */
bool qmdiClient::canCloseClient() { return true; }

/**
 * \brief The file opened by this MDI client
 * \return by default an empth string.
 *
 * This function returns the file name which is opened by the MDI client.
 * The file name is not really used inside \b qmdilib , but it's left
 * for usage outside of library. The \b name property should reflect
 * this property, but it's not really needed.
 *
 * By default this function returns an empty string, but you should
 * re-implement in derived functions if you want to properly work
 * with mdi clients.
 *
 * The reason for the prefix of this method, is that on derived implementations
 * (a QObject implementing this interface) there might be a method with a
 * similar name.
 *
 * \since 0.0.5
 */
QString qmdiClient::mdiClientFileName() { return QString(); }

/**
 * \brief Callback function to announce that the client has been merged
 * \param host the host which created this event
 *
 * This method is fired up by qmdiHost::mergeClient(),
 * to announce that this client has been merged at the
 * host passed as a parameter.
 *
 * This function adds adds all menu/toolbar actions to the host. Please do call this method 
 * when re-implementing.
 *
 * \see qmdiHost::mergeClient()
 * \since 0.0.5
 */
void qmdiClient::on_client_merged(qmdiHost *host) { 
    if (auto w = dynamic_cast<QWidget*>(host)) {
        menus.addActionsToWidget(w);
        toolbars.addActionsToWidget(w);
    }
}

/**
 * \brief Callback function to announce that the client has been unmerged
 * \param host the host which created this event
 *
 * This method is fired up by qmdiHost::unmergeClient(),
 * to announce that this client has been merged at the
 * host passed as a parameter.
 *
 * This function adds removes all menu/toolbar actions to the host. Please do call this method 
 * when re-implementing.
 *
 * \see qmdiHost::unmergeClient()
 * \since 0.0.5
 */
void qmdiClient::on_client_unmerged(qmdiHost *host) { 
    if (auto w = dynamic_cast<QWidget*>(host)) {
        menus.removeActionsFromWidget(w);
        toolbars.removeActionsFromWidget(w);
    }
}

/**
 * @brief Return internal state
 *
 * When you request a PluginManager to add a new client, you pass 2 ints. Those ints have no special
 * meaning, and it up to the client to use them. In text editors, this can be used to save
 * determinate the column/row of the cursor position.
 *
 * This function is the exact oposite - it queries the client for those 3 intergers. This logic
 * is not enforced by the library, and its up to the developer to use as he needs.
 *
 * Default implementation returns an invalid optional.
 *
 * \see PluginManager::openFile()
 * \since 0.1.1
 */
std::optional<std::tuple<int, int, int>> qmdiClient::get_coordinates() const { return {}; }
