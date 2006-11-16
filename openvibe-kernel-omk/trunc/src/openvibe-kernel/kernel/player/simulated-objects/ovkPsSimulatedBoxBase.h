/************************************************************************/
/* This file is part of openMask(c) INRIA, CNRS, Universite de Rennes 1 */
/* 1993-2002, thereinafter the Software                                 */
/*                                                                      */
/* The Software has been developped within the Siames Project.          */
/* INRIA, the University of Rennes 1 and CNRS jointly hold intellectual */
/* property rights                                                      */
/*                                                                      */
/* The Software has been registered with the Agence pour la Protection  */
/* des Programmes (APP) under registration number                       */
/* IDDN.FR.001.510008.00.S.P.2001.000.41200                             */
/*                                                                      */
/* This file may be distributed under the terms of the Q Public License */
/* version 1.0 as defined by Trolltech AS of Norway and appearing in    */
/* the file LICENSE.QPL included in the packaging of this file.         */
/*                                                                      */
/* Licensees holding valid specific licenses issued by INRIA, CNRS or   */
/* Universite Rennes 1 for the software may use this file in            */
/* acordance with that specific license                                 */
/************************************************************************/
/************************************************************************/
/* WARNING : GENERATED FILE. DO NOT MODIFY IT.                          */
/* Generated the 2006-09-12 at 18:00:28                                 */
/* Generated by omk version 1.2.0                                       */
/* Use just as it is or derive this class to add the  necessary         */
/* functions.                                                           */
/************************************************************************/
/************************************************************************/

#if !defined _SIMULATED_OBJECTS_OVKPSSIMULATEDBOXBASE_H_
#define _SIMULATED_OBJECTS_OVKPSSIMULATEDBOXBASE_H_

#include <PsSimulatedObject.h>
#include <PsValuedEventListenerCallBack.h>
#include <PsEventListenerCallBack.h>
#include "../ovkEventId.h"
#include "../types/ovkPsTypeChunk.h"

/** \brief Class \ref PsSimulatedBoxBase generated.
 * \date generated the 2006-09-12 at 18:00:28
 * \author Yann Renard (INRIA/IRISA)
 * @description
 * This class is generated. \n
 * Use it by derivation to add the necessary methods.\n
 * This base class is never implemented, only the derivate classes (like \ref PsSimulatedBox) are.\n
 * All the members of this class are protected except the destructor according to the OpenMASK precepts.\n
 * @configurationParameters
 * \n \b Connection \b parameters \b for \b events \n
 * The following connection parameters are optional, they are used to listen signals.
 * \li \b ListenOpenViBEDataUpdate a boolean, \e false by default, it tells if the 
 * signal "OpenViBEDataUpdate" is listened (\e true) or not (\e false).
 * \li \b ListenOpenViBEDataUpdateBy a array of object names, the signal 
 * "OpenViBEDataUpdate" which emitted by those objects will be listened.
 * \li \b ListenOpenViBESetBoxPrototype a boolean, \e false by default, it tells if the 
 * signal "OpenViBESetBoxPrototype" is listened (\e true) or not (\e false).
 * \li \b ListenOpenViBESetBoxPrototypeBy a array of object names, the signal 
 * "OpenViBESetBoxPrototype" which emitted by those objects will be listened.
 *
 * \n \b Configuration \b example
 * \code
 myObject
 {
   Class PsSimulatedBox
   Scheduling
   {
     Frequency 75
     Process processA
   }
   UserParams
   {
     // Optional parameters
     ListenOpenViBEDataUpdate false
     ListenOpenViBEDataUpdateBy [ObjectName AnotherObjectName]
     ListenOpenViBESetBoxPrototype false
     ListenOpenViBESetBoxPrototypeBy [ObjectName AnotherObjectName]

 }
 * \endcode
 * \receivedEvents
 * \li \b OpenViBEDataUpdate:  a valued event by \e PsTypeChunk.
 * This event is sent between the boxes in order so they can exchange buffer updates
 * \n To listen this signal add the following lines to your configuration
 * file. The first is to listen the signal from any object, the second is
 * to listen the signal from object named \e emittorObject.
 * \code
     ListenOpenViBEDataUpdate true
     ListenOpenViBEDataUpdateBy [emittorObject]
 * \endcode
 * \li \b OpenViBESetBoxPrototype:  a valued event by \e PsTypeBoxPrototype.
 * This event is sent from the engine to configure each box' interface
 * \n To listen this signal add the following lines to your configuration
 * file. The first is to listen the signal from any object, the second is
 * to listen the signal from object named \e emittorObject.
 * \code
     ListenOpenViBESetBoxPrototype true
     ListenOpenViBESetBoxPrototypeBy [emittorObject]
 * \endcode
 */
class PsSimulatedBoxBase : public PsSimulatedObject
{
public:

	/// @name Constructors and destructor.
	//@{
	/// \brief Destructor of \ref PsSimulatedBoxBase.
	virtual ~PsSimulatedBoxBase() ;

protected:

	/// \brief Protected constructor of \ref PsSimulatedBoxBase call by derivate classes (\ref PsSimulatedBox).
	PsSimulatedBoxBase( PsController& ctrl, const PsObjectDescriptor& objectDescriptor );
	//@}

	/// @name Reset and loaders
	//@{
	/// \brief Reset the default values of \ref PsSimulatedBoxBase.
	///
	/// No data or output have been declared, this method does nothing.
	virtual void resetDefaultValues();
	/// \brief Configuration parameters loader of \ref PsSimulatedBoxBase.
	/// \param[in] node the root node of the configuration parameter.
	/// \return true if all \e needed parameters can be read.
	///
	/// Reads in the configuration node the values to set the attributs.
	/// No data or output have been declared, this method does nothing.
	virtual bool loadAttributsParameters( const PsConfigurationParameterDescriptor * node );
	/// \brief Configuration parameters loader of \ref PsSimulatedBoxBase.
	/// \param[in] node the root node of the configuration parameter.
	/// \return true if all the inputs are well connected.
	///
	/// Reads in the configuration node the values to connect the inputs.
	/// No input has been declared, this method does nothing.
	virtual bool loadInputsConnectionsParameters( const PsConfigurationParameterDescriptor * node ) { return node!=NULL ; }
	/// \brief Configuration parameters loader of \ref PsSimulatedBoxBase.
	/// \param[in] node the root node of the configuration parameter.
	/// \return true if all the events are well connected to a signal.
	///
	/// Creates events listeners and associates them to their callback method.\n
	/// Reads in the configuration node the values to register signals.
	virtual bool loadEventsConnectionsParameters( const PsConfigurationParameterDescriptor * node ) ;
	//@}

	/// @name PsSimulatedObject interfaces
	//@{
	/// \brief Init method inherited from PsSimulatedObject.
	///
	/// Calls the loaders.\n
	/// Most of the time it is not necessary to redefine this method in the children
	/// class. If you have to add some initialisations, redefine and call the
	/// inherited method to load parameters or call directly the loaders.
	virtual void init() ;
	/// \brief Compute method inherited from PsSimulatedObject.
	///
	/// The \ref compute method is splitted in three parts: the first reads the
	/// inputs, the second computes the new values for the parameters, the last
	/// one sets the outputs from their associated parmeters.\n
	/// In the children class, the second method must be defined, and most of
	/// the time you do not have to change the others.\n
	virtual void compute() ;
	/// \brief This part of the \ref compute methods reads the inputs.
	///
	/// No input has been declared, nothing to do in this method.
	virtual void computeInputs() {}
	/// \brief This part of the \ref compute methods computes the parameters.
	///
	/// The inputs and outputs works with associated data, you should use their associated data.\n
	/// It is a pure virtual method, so it must be defined in the children.\n
	/// This method  updates nothing.
	virtual void computeParameters() = 0 ;
	/// \brief This part of the \ref compute methods copies the data in their associated output.
	///
	/// No output has been declared, nothing to do in this method.
	virtual void computeOutputs() {}
	//@}

protected:

	/// @name Call back methods and listeners for events processing.
	//@{
	/// \brief Call back to process the "OpenViBEDataUpdate" event
	/// \return see \ref PsValuedEventListenerCallBack::CallBackFct
	virtual bool processOpenViBEDataUpdateEvent( ::PsValuedEvent< ::PsTypeChunk > *e ) ;
	/// \brief Event listener to listen the "OpenViBEDataUpdate" event.
	///
	/// The event listener is created in the \ref init method.\n
	/// Use registration methods \ref registerForOpenViBEDataUpdateSignal and
	/// registerForOpenViBEDataUpdateSignalBy to listen the signal.
	PsValuedEventListenerCallBack< ::PsSimulatedBoxBase, ::PsTypeChunk > *_openViBEDataUpdateEventListener ;
	//@}

	/// @name Registering signals.
	//@{
	/// \brief Register listening of the "OpenViBEDataUpdate" signal comming from any object.
	/// \return true if the registering is ok
	bool registerForOpenViBEDataUpdateSignal() { return registerForSignal( EventId::s_openViBEDataUpdate ) ; }
	/// \brief Listening cancel of the "OpenViBEDataUpdate" signal from any object
	/// previously registered with \ref registerForOpenViBEDataUpdateSignal.
	/// \return true if the canceling is ok
	bool cancelRegistrationForOpenViBEDataUpdateSignal() { return cancelRegistrationForSignal( EventId::s_openViBEDataUpdate ) ; }
	/// \brief Register listening of the "OpenViBEDataUpdate" signal comming from specified object.
	/// \param[in] producer the object to listen
	/// \return true if the registering is ok
	bool registerForOpenViBEDataUpdateSignalBy( const PsName &producer ) { return registerForSignalBy( EventId::s_openViBEDataUpdate, producer  ) ; }
	/// \brief Listening cancel of the "OpenViBEDataUpdate" signal from specified object
	/// previously registered with \ref registerForOpenViBEDataUpdateSignalBy.
	/// \param[in] producer the listened object to cancel from.
	/// \return true if the canceling is ok
	bool cancelRegistrationForOpenViBEDataUpdateSignalBy( const PsName &producer ) { return cancelRegistrationForSignalBy( EventId::s_openViBEDataUpdate, producer ) ; }
	//@}

	/// @name Methods to emit event and signal.
	//@{
	/// \brief Sends the "OpenViBEDataUpdate" event to a specific object
	/// \param[in] receiver the name of the receiver object
	/// \param[in] value the value associated to the event
	void sendOpenViBEDataUpdateEvent( const PsName &receiver, const PsTypeChunk & value )
	{ sendValuedEvent( receiver, EventId::s_openViBEDataUpdate, value ) ; }
	/// \brief Sends the "OpenViBEDataUpdate" signal.
	/// \param[in] value the value associated to the signal
	void fireOpenViBEDataUpdateEvent( const PsTypeChunk & value )
	{ fireValuedSignal( EventId::s_openViBEDataUpdate, value ) ; }
	//@}

protected:

	///Flag to know how to handle error during init.
	bool _exitOnInitError ;
	/// @name Data
	//@{

	//@}
	/// @name Inputs, outputs and configuration parameters
	/// The derivated classes must uses the data
	//@{
	//@}
};

#endif // defined _SIMULATED_OBJECTS_OVKPSSIMULATEDBOXBASE_H_
