#ifndef _SerialPort_h_
#    include <SerialPort.h>
#endif

#ifndef _termios_h_INCLUDED_
#    include <termios.h>
#    define _termios_h_INCLUDED_
#endif

#ifndef _fcntl_h_INCLUDED_
#    include <fcntl.h>
#    define _fcntl_h_INCLUDED_
#endif

#ifndef _unistd_h_INCLUDED_
#    include <unistd.h>
#    define _unistd_h_INCLUDED_
#endif

#ifndef _sys__ioctl_h_INCLUDED_
#    include <sys/ioctl.h>
#    define _sys__ioctl_h_INCLUDED_
#endif

#ifndef _std_cerrno_INCLUDED_
#    include <cerrno>
#    define _std_cerrno_INCLUDED_
#endif

#ifndef _std_cassert_INCLUDED_
#    include <cassert>
#    define _std_cassert_INCLUDED_
#endif

namespace {
    //
    // Various error messages used in this file while throwing
    // exceptions.
    //
    const std::string ERR_MSG_PORT_NOT_OPEN     = "Serial port not open." ;
    const std::string ERR_MSG_PORT_ALREADY_OPEN = "Serial port already open." ;
    const std::string ERR_MSG_UNSUPPORTED_BAUD  = "Unsupported baud rate." ;
    const std::string ERR_MSG_UNKNOWN_BAUD      = "Unknown baud rate." ;
    const std::string ERR_MSG_INVALID_PARITY    = "Invalid parity setting." ;
    const std::string ERR_MSG_INVALID_STOP_BITS = "Invalid number of stop bits." ;
    const std::string ERR_MSG_INVALID_FLOW_CONTROL = "Invalid flow control." ;
} ;

class SerialPortImpl {
public:
    /**
     * Constructor.
     */
    SerialPortImpl( const std::string& serialPortName ) ;

    /**
     * Destructor.
     */
    ~SerialPortImpl() ;

    /**
     * Open the serial port. 
     */
    void Open() 
        throw( SerialPort::OpenFailed,
               SerialPort::AlreadyOpen ) ;

    /**
     * Check if the serial port is currently open.
     */
    bool 
    IsOpen() const ;

    /**
     * Close the serial port. 
     */
    void
    Close() 
        throw(SerialPort::NotOpen) ;

    /**
     * Set the baud rate of the serial port.
     */
    void
    SetBaudRate( const SerialPort::BaudRate baudRate )
        throw( SerialPort::NotOpen, 
               SerialPort::UnsupportedBaudRate,
               std::invalid_argument, 
               std::runtime_error ) ;

    /**
     * Get the current baud rate. 
     */
    SerialPort::BaudRate
    GetBaudRate() const 
        throw( SerialPort::NotOpen,
               std::runtime_error ) ;

    /**
     * Set the character size.
     */
    void
    SetCharSize( const SerialPort::CharacterSize charSize )
        throw( SerialPort::NotOpen, 
               std::invalid_argument,
               std::runtime_error ) ;

    /**
     * Get the current character size.
     */
    SerialPort::CharacterSize
    GetCharSize() const
        throw( SerialPort::NotOpen,
               std::runtime_error )  ;

    void
    SetParity( const SerialPort::Parity parityType )
        throw( SerialPort::NotOpen,
               std::invalid_argument, 
               std::runtime_error )  ;

    SerialPort::Parity
    GetParity() const 
        throw(SerialPort::NotOpen) ;

    void
    SetNumOfStopBits( const SerialPort::StopBits numOfStopBits ) 
        throw( SerialPort::NotOpen,
               std::invalid_argument )  ;

    SerialPort::StopBits
    GetNumOfStopBits() const 
        throw(SerialPort::NotOpen) ;

    void
    SetFlowControl( const SerialPort::FlowControl flowControl ) 
        throw( SerialPort::NotOpen,
               std::invalid_argument ) ;

    SerialPort::FlowControl
    GetFlowControl() const 
        throw( SerialPort::NotOpen ) ;

    bool
    IsDataAvailable() const 
        throw( SerialPort::NotOpen,
               std::runtime_error ) ;

    unsigned char
    ReadByte(const unsigned int msTimeout = 0 )
        throw( SerialPort::NotOpen,
               SerialPort::ReadTimeout,
               std::runtime_error ) ;

    void
    Read( SerialPort::DataBuffer& dataBuffer,
          const unsigned int      numOfBytes, 
          const unsigned int      msTimeout ) 
        throw( SerialPort::NotOpen, 
               SerialPort::ReadTimeout, 
               std::runtime_error  ) ;
        
    void
    WriteByte( const unsigned char dataByte )
        throw( SerialPort::NotOpen,
               std::runtime_error ) ;

    void 
    SerialPortImpl::Write(const SerialPort::DataBuffer& dataBuffer)
        throw( SerialPort::NotOpen, 
               std::runtime_error ) ;

    void 
    SerialPortImpl::Write( const unsigned char* dataBuffer, 
                           const unsigned int   bufferSize )
        throw( SerialPort::NotOpen, 
               std::runtime_error ) ; 
private:
    /**
     * Name of the serial port. On POSIX systems this is the name of
     * the device file.
     */
    std::string mSerialPortName ;

    /**
     * Flag that indicates whether the serial port is currently open.
     */
    bool mIsOpen ;
    
    /**
     * The file descriptor corresponding to the serial port.
     */
    int mFileDescriptor ;

    /**
     * Serial port settings are saved into this variable immediately
     * after the port is opened. These settings are restored when the
     * serial port is closed.
     */
    termios mOldPortSettings ;

    /**
     * Set the timeout for the next read to msTimeout milliseconds. If
     * msTimeout is zero, then the reads will block until atleast
     * numOfBytes bytes are received. 
     *
     * :TRICKY: This method does not check if the serial port is
     * currently open. Hence, it should not be called without making
     * sure that the serial port is open.
     */
    void SetReadTimeout( const unsigned int msTimeout,
                         const unsigned int numOfBytes = 0 ) 
        throw( std::invalid_argument, 
               std::runtime_error ) ;
    
} ;

SerialPort::SerialPort( const std::string& serialPortName ) :
    mSerialPortImpl(new SerialPortImpl(serialPortName) ) 
{
    /* empty */
}

SerialPort::~SerialPort() 
    throw()
{
    /*
     * Close the serial port if it is open.
     */
    if ( this->IsOpen() ) {
        this->Close() ;
    }
    /*
     * Free the memory allocated to the implementation instance.
     */
    if ( mSerialPortImpl ) {
        delete mSerialPortImpl ;
    }
    return ;
}

void
SerialPort::Open( const BaudRate      baudRate,
                  const CharacterSize charSize,
                  const Parity        parityType,
                  const StopBits      stopBits,
                  const FlowControl   flowControl )
    throw( OpenFailed,
           AlreadyOpen,
           UnsupportedBaudRate,
           std::invalid_argument ) 
{
    //
    // Open the serial port. 
    mSerialPortImpl->Open() ;
    //
    // Set the various parameters of the serial port if it is open.
    this->SetBaudRate(baudRate) ;
    this->SetCharSize(charSize) ;
    this->SetParity(parityType) ;
    this->SetNumOfStopBits(stopBits) ;
    this->SetFlowControl(flowControl) ;
    //
    // All done.
    return ;
}

bool
SerialPort::IsOpen() const
{
    return mSerialPortImpl->IsOpen() ;
}

void
SerialPort::Close()
    throw(NotOpen) 
{
    mSerialPortImpl->Close() ;
    return ;
}

void
SerialPort::SetBaudRate( const BaudRate baudRate )
    throw( UnsupportedBaudRate,
           NotOpen, 
           std::invalid_argument ) 
{
    mSerialPortImpl->SetBaudRate( baudRate ) ;
    return ;
}

SerialPort::BaudRate
SerialPort::GetBaudRate() const 
    throw( NotOpen,
           std::runtime_error )
{
    return mSerialPortImpl->GetBaudRate() ;
}


void
SerialPort::SetCharSize( const CharacterSize charSize )
    throw( NotOpen, 
           std::invalid_argument ) 
{
    mSerialPortImpl->SetCharSize(charSize) ;
}

SerialPort::CharacterSize
SerialPort::GetCharSize() const 
    throw(NotOpen) 
{
    return mSerialPortImpl->GetCharSize() ;
}

void
SerialPort::SetParity( const Parity parityType ) 
    throw( NotOpen,
           std::invalid_argument ) 
{
    mSerialPortImpl->SetParity( parityType ) ;
    return ;
}

SerialPort::Parity
SerialPort::GetParity() const 
    throw(NotOpen) 
{
    return mSerialPortImpl->GetParity() ;
}

void
SerialPort::SetNumOfStopBits( const StopBits numOfStopBits ) 
    throw( NotOpen,
           std::invalid_argument ) 
{
    mSerialPortImpl->SetNumOfStopBits(numOfStopBits) ;
    return ;
}

SerialPort::StopBits
SerialPort::GetNumOfStopBits() const 
    throw(NotOpen) 
{
    return mSerialPortImpl->GetNumOfStopBits() ;
}


void
SerialPort::SetFlowControl( const FlowControl   flowControl ) 
    throw( NotOpen,
           std::invalid_argument ) 
{
    mSerialPortImpl->SetFlowControl( flowControl ) ;
    return ;
}

SerialPort::FlowControl
SerialPort::GetFlowControl() const 
    throw( NotOpen ) 
{
    return mSerialPortImpl->GetFlowControl() ;
}

bool 
SerialPort::IsDataAvailable() const
    throw(NotOpen) 
{
    return mSerialPortImpl->IsDataAvailable() ;
}

unsigned char
SerialPort::ReadByte( const unsigned int msTimeout )
    throw( NotOpen,
           ReadTimeout,
           std::runtime_error )
{
    return mSerialPortImpl->ReadByte(msTimeout) ;
}

void
SerialPort::Read( SerialPort::DataBuffer& dataBuffer, 
                  const unsigned int      numOfBytes, 
                  const unsigned int      msTimeout ) 
    throw( NotOpen, 
           ReadTimeout,
           std::runtime_error ) 
{
    return mSerialPortImpl->Read( dataBuffer, 
                                  numOfBytes, 
                                  msTimeout ) ;
}

const std::string 
SerialPort::ReadLine( const unsigned int msTimeout,
                      const char         lineTerminator ) 
    throw( NotOpen, 
           ReadTimeout,
           std::runtime_error ) 
{
    std::string result ;
    char next_char = 0 ; 
    do {
        next_char = this->ReadByte( msTimeout ) ;
        result += next_char ;
    } while( next_char != lineTerminator ) ;
    return result ;
}

void
SerialPort::WriteByte( const unsigned char dataByte )
    throw( SerialPort::NotOpen,
           std::runtime_error )
{
    mSerialPortImpl->WriteByte( dataByte ) ;
    return ;
} 


void 
SerialPort::Write(const DataBuffer& dataBuffer)
    throw( NotOpen, 
           std::runtime_error ) 
{
    mSerialPortImpl->Write( dataBuffer ) ;
    return ;
}

void
SerialPort::Write(const std::string& dataString) 
    throw( NotOpen, 
           std::runtime_error ) 
{
    mSerialPortImpl->Write( reinterpret_cast<const unsigned char*>(dataString.c_str()),
                            dataString.length() ) ;
    return ;
}

/* ------------------------------------------------------------ */
inline
SerialPortImpl::SerialPortImpl( const std::string& serialPortName ) :
    mSerialPortName(serialPortName),
    mIsOpen(false),
    mFileDescriptor(-1), 
    mOldPortSettings()
{
    /* empty */
}

inline
SerialPortImpl::~SerialPortImpl()
{
    //
    // Close the serial port if it is open.
    //
    if ( this->IsOpen() ) {
        this->Close() ;
    }
    return ;
}

inline
void
SerialPortImpl::Open()
    throw( SerialPort::OpenFailed,
           SerialPort::AlreadyOpen ) 
{
    /*
     * Throw an exception if the port is already open.
     */
    if ( this->IsOpen() ) {
        throw SerialPort::AlreadyOpen( ERR_MSG_PORT_ALREADY_OPEN ) ;
    }
    /*
     * Try to open the serial port and throw an exception if we are
     * not able to open it.
     */
    mFileDescriptor = open( mSerialPortName.c_str(), 
                            O_RDWR | O_NOCTTY ) ; 
    if ( mFileDescriptor < 0 ) {
        throw SerialPort::OpenFailed( strerror(errno) )  ;
    } 
    /*
     * Save the current settings of the serial port so they can be
     * restored when the serial port is closed.
     */
    if ( tcgetattr( mFileDescriptor, 
                    &mOldPortSettings ) < 0 ) {
        throw SerialPort::OpenFailed( strerror(errno) ) ;
    }

    //
    // Copy the old settings and modify them as necessary.
    //
    termios port_settings = mOldPortSettings ;
    
    //
    // Zero out all local and output flags. 
    //
    port_settings.c_iflag = 0 ;
    port_settings.c_lflag = 0 ;
    port_settings.c_oflag = 0 ;
    
    //
    // Enable the receiver (CREAD) and ignore modem control lines
    // (CLOCAL).
    //
    port_settings.c_cflag |= CREAD | CLOCAL ;

    //
    // Set the VMIN and VTIME parameters to zero by default. VMIN is
    // the minimum number of characters for non-canonical read and
    // VTIME is the timeout in deciseconds for non-canonical
    // read. Setting both of these parameters to zero implies that a
    // read will return immediately only giving the currently
    // available characters.
    //
    port_settings.c_cc[ VMIN  ] = 0 ;
    port_settings.c_cc[ VTIME ] = 0 ;
    
    /*
     * Write the new settings to the port.
     */
    if ( tcsetattr( mFileDescriptor, 
                    TCSANOW,
                    &port_settings ) < 0 ) {
        throw SerialPort::OpenFailed( strerror(errno) ) ;
    }

    /*
     * Direct all SIGIO and SIGURG signals for the port to the current
     * process.
     */
    if ( fcntl( mFileDescriptor, 
                F_SETOWN, 
                getpid() ) < 0 ) {
        throw SerialPort::OpenFailed( strerror(errno) ) ;
    }

    /*
     * The serial port is open at this point.
     */
    mIsOpen = true ;
    return ;
}

inline
bool
SerialPortImpl::IsOpen() const
{
    return mIsOpen ;
}

inline
void
SerialPortImpl::Close() 
    throw( SerialPort::NotOpen )
{
    //
    // Throw an exception if the serial port is not open.
    //
    if ( ! this->IsOpen() ) {
        throw SerialPort::NotOpen( ERR_MSG_PORT_NOT_OPEN ) ;
    }
    //
    // Restore the old settings of the port. 
    //
    tcsetattr( mFileDescriptor, 
               TCSANOW, 
               &mOldPortSettings ) ;
    //
    // Close the serial port file descriptor. 
    //
    close(mFileDescriptor) ;
    //
    // The port is not open anymore.
    //
    mIsOpen = false ;
    //
    return ;
}

inline
void
SerialPortImpl::SetBaudRate( const SerialPort::BaudRate baudRate )
    throw( SerialPort::NotOpen, 
           SerialPort::UnsupportedBaudRate,
           std::invalid_argument,
           std::runtime_error ) 
{
    //
    // Throw an exception if the serial port is not open.
    //
    if ( ! this->IsOpen() ) {
        throw SerialPort::NotOpen( ERR_MSG_PORT_NOT_OPEN ) ;
    }
    //
    // Get the current settings of the serial port.
    //
    termios port_settings ;
    if ( tcgetattr( mFileDescriptor, 
                    &port_settings ) < 0 ) {
        throw std::runtime_error( strerror(errno) ) ;
    }
    //
    // Set the baud rate for both input and output.
    //
    if ( ( cfsetispeed( &port_settings, 
                        baudRate ) < 0 ) ||
         ( cfsetospeed( &port_settings, 
                        baudRate ) < 0 ) ) {
        //
        // If any of the settings fail, we abandon this method.
        //
        throw SerialPort::UnsupportedBaudRate( ERR_MSG_UNSUPPORTED_BAUD ) ;
    }
    //
    // Set the new attributes of the serial port. 
    //
    if ( tcsetattr( mFileDescriptor, 
                    TCSANOW, 
                    &port_settings ) < 0 ) {
        throw SerialPort::UnsupportedBaudRate( strerror(errno) ) ;
    }
    return ;
}

SerialPort::BaudRate
SerialPortImpl::GetBaudRate() const
    throw( SerialPort::NotOpen, 
           std::runtime_error )
{
    //
    // Make sure that the serial port is open.
    //
    if ( ! this->IsOpen() ) {
        throw SerialPort::NotOpen( ERR_MSG_PORT_NOT_OPEN ) ;
    }
    //
    // Read the current serial port settings. 
    //
    termios port_settings ;
    if ( tcgetattr( mFileDescriptor, 
                    &port_settings ) < 0 ) {
        throw std::runtime_error( strerror(errno) ) ;
    }
    //
    // Obtain the input baud rate from the current settings.
    //
    return SerialPort::BaudRate(cfgetispeed( &port_settings )) ;
}

void
SerialPortImpl::SetCharSize( const SerialPort::CharacterSize charSize )
    throw( SerialPort::NotOpen, 
           std::invalid_argument,
           std::runtime_error ) 
{
    //
    // Make sure that the serial port is open. 
    //
    if ( ! this->IsOpen() ) {
        throw SerialPort::NotOpen( ERR_MSG_PORT_NOT_OPEN ) ;
    }
    //
    // Get the current settings of the serial port.
    //
    termios port_settings ;
    if ( tcgetattr( mFileDescriptor, 
                    &port_settings ) < 0 ) {
        throw std::runtime_error( strerror(errno) ) ;
    }
    //
    // Set the character size. 
    //
    port_settings.c_cflag &= ~CSIZE ;
    port_settings.c_cflag |= charSize ;
    //
    // Apply the modified settings. 
    //
    if ( tcsetattr( mFileDescriptor, 
                    TCSANOW, 
                    &port_settings ) < 0 ) {
        throw std::invalid_argument( strerror(errno) ) ;
    }
    return ;
}

SerialPort::CharacterSize
SerialPortImpl::GetCharSize() const 
    throw( SerialPort::NotOpen, 
           std::runtime_error ) 
{
    //
    // Make sure that the serial port is open. 
    //
    if ( ! this->IsOpen() ) {
        throw SerialPort::NotOpen( ERR_MSG_PORT_NOT_OPEN ) ;
    }
    //
    // Get the current port settings. 
    //
    termios port_settings ;
    if ( tcgetattr( mFileDescriptor, 
                    &port_settings ) < 0 ) {
        throw std::runtime_error( strerror(errno) ) ;
    }
    //
    // Read the character size from the setttings. 
    //
    return SerialPort::CharacterSize( port_settings.c_cflag & CSIZE ) ;
}

void
SerialPortImpl::SetParity( const SerialPort::Parity parityType )
    throw( SerialPort::NotOpen,
           std::invalid_argument, 
           std::runtime_error ) 
{
    //
    // Make sure that the serial port is open. 
    //
    if ( ! this->IsOpen() ) {
        throw SerialPort::NotOpen( ERR_MSG_PORT_NOT_OPEN ) ;
    }
    //
    // Get the current port settings. 
    //
    termios port_settings ;
    if ( tcgetattr( mFileDescriptor, 
                    &port_settings ) < 0 ) {
        throw std::runtime_error( strerror(errno) ) ;
    }
    //
    // Set the parity type depending on the specified parameter. 
    //
    switch( parityType ) {
    case SerialPort::PARITY_EVEN:
        port_settings.c_cflag |= PARENB ;
        port_settings.c_cflag &= ~PARODD ;
        break ;
    case SerialPort::PARITY_ODD:
        port_settings.c_cflag |= ( PARENB | PARODD ) ;
        break ;
    case SerialPort::PARITY_NONE:
        port_settings.c_cflag &= ~(PARENB) ;
        break ;
    default:
        throw std::invalid_argument( ERR_MSG_INVALID_PARITY ) ;
        break ;
    }
    //
    // Apply the modified port settings. 
    //
    if ( tcsetattr( mFileDescriptor, 
                    TCSANOW, 
                    &port_settings ) < 0 ) {
        throw std::invalid_argument( strerror(errno) ) ;
    }
    return ;
}

SerialPort::Parity
SerialPortImpl::GetParity() const 
    throw(SerialPort::NotOpen) 
{
    //
    // Make sure that the serial port is open. 
    //
    if ( ! this->IsOpen() ) {
        throw SerialPort::NotOpen( ERR_MSG_PORT_NOT_OPEN ) ;
    }
    //
    // Get the current port settings. 
    //
    termios port_settings ;
    if ( tcgetattr( mFileDescriptor, 
                    &port_settings ) < 0 ) {
        throw std::runtime_error( strerror(errno) ) ;
    }
    //
    // Get the parity type from the current settings.
    //
    if ( port_settings.c_cflag & PARENB ) {
        //
        // Parity is enabled. Lets check if it is odd or even. 
        //
        if ( port_settings.c_cflag & PARODD ) {
            return SerialPort::PARITY_ODD ;
        } else {
            return SerialPort::PARITY_EVEN ;
        }
    } 
    //
    // Parity is disabled.
    //
    return SerialPort::PARITY_NONE ;
}

void
SerialPortImpl::SetNumOfStopBits( const SerialPort::StopBits numOfStopBits ) 
    throw( SerialPort::NotOpen,
           std::invalid_argument ) 
{
    //
    // Make sure that the serial port is open. 
    //
    if ( ! this->IsOpen() ) {
        throw SerialPort::NotOpen( ERR_MSG_PORT_NOT_OPEN ) ;
    }
    //
    // Get the current port settings. 
    //
    termios port_settings ;
    if ( tcgetattr( mFileDescriptor, 
                    &port_settings ) < 0 ) {
        throw std::runtime_error( strerror(errno) ) ;
    }
    //
    // Set the number of stop bits. 
    //
    switch( numOfStopBits ) {
    case SerialPort::STOP_BITS_1:
        port_settings.c_cflag &= ~(CSTOPB) ;
        break ;
    case SerialPort::STOP_BITS_2:
        port_settings.c_cflag |= CSTOPB ;
        break ;        
    default:
        throw std::invalid_argument( ERR_MSG_INVALID_STOP_BITS ) ;
        break ;
    } 
    //
    // Apply the modified settings. 
    //
    if ( tcsetattr( mFileDescriptor, 
                    TCSANOW, 
                    &port_settings ) < 0 ) {
        throw std::invalid_argument( strerror(errno) ) ;
    }
    return ;
}

SerialPort::StopBits
SerialPortImpl::GetNumOfStopBits() const 
    throw(SerialPort::NotOpen) 
{
    //
    // Make sure that the serial port is open. 
    //
    if ( ! this->IsOpen() ) {
        throw SerialPort::NotOpen( ERR_MSG_PORT_NOT_OPEN ) ;
    }
    //
    // Get the current port settings. 
    //
    termios port_settings ;
    if ( tcgetattr( mFileDescriptor, 
                    &port_settings ) < 0 ) {
        throw std::runtime_error( strerror(errno) ) ;
    }
    //
    // If CSTOPB is set then we are using two stop bits, otherwise we
    // are using 1 stop bit.
    //
    if ( port_settings.c_cflag & CSTOPB ) {
        return SerialPort::STOP_BITS_2 ;
    }
    return SerialPort::STOP_BITS_1 ;
}

void
SerialPortImpl::SetFlowControl( const SerialPort::FlowControl   flowControl ) 
    throw( SerialPort::NotOpen,
           std::invalid_argument ) 
{
    //
    // Make sure that the serial port is open. 
    //
    if ( ! this->IsOpen() ) {
        throw SerialPort::NotOpen( ERR_MSG_PORT_NOT_OPEN ) ;
    }
    //
    // Get the current port settings. 
    //
    termios port_settings ;
    if ( tcgetattr( mFileDescriptor, 
                    &port_settings ) < 0 ) {
        throw std::runtime_error( strerror(errno) ) ;
    }
    //
    // Set the flow control.
    //
    switch( flowControl ) {
    case SerialPort::FLOW_CONTROL_HARD:
        port_settings.c_cflag |= CRTSCTS ;
        break ;
    case SerialPort::FLOW_CONTROL_NONE:
        port_settings.c_cflag &= ~(CRTSCTS) ;
        break ;
    default:
        throw std::invalid_argument( ERR_MSG_INVALID_FLOW_CONTROL ) ;
        break ;
    }
    // 
    // Apply the modified settings. 
    //
    if ( tcsetattr( mFileDescriptor, 
                    TCSANOW, 
                    &port_settings ) < 0 ) {
        throw std::invalid_argument( strerror(errno) ) ;
    }    
    return ;
}

SerialPort::FlowControl
SerialPortImpl::GetFlowControl() const 
    throw( SerialPort::NotOpen ) 
{
    //
    // Make sure that the serial port is open. 
    //
    if ( ! this->IsOpen() ) {
        throw SerialPort::NotOpen( ERR_MSG_PORT_NOT_OPEN ) ;
    }
    //
    // Get the current port settings. 
    //
    termios port_settings ;
    if ( tcgetattr( mFileDescriptor, 
                    &port_settings ) < 0 ) {
        throw std::runtime_error( strerror(errno) ) ;
    }
    //
    // If CRTSCTS is set then we are using hardware flow
    // control. Otherwise, we are not using any flow control.
    //
    if ( port_settings.c_cflag & CRTSCTS ) {
        return SerialPort::FLOW_CONTROL_HARD ;
    }
    return SerialPort::FLOW_CONTROL_NONE ;
}

bool
SerialPortImpl::IsDataAvailable() const 
    throw( SerialPort::NotOpen, 
           std::runtime_error ) 
{
    //
    // Make sure that the serial port is open. 
    //
    if ( ! this->IsOpen() ) {
        throw SerialPort::NotOpen( ERR_MSG_PORT_NOT_OPEN ) ;
    }
    //
    // Check if any data is available at the serial port. 
    //
    int num_of_bytes_available = 0 ;
    if ( ioctl( mFileDescriptor, 
                FIONREAD, 
                &num_of_bytes_available ) < 0 ) {
        throw std::runtime_error( strerror(errno) ) ;
    }
    return (num_of_bytes_available ? true : false) ;
}

unsigned char
SerialPortImpl::ReadByte(const unsigned int msTimeout)
    throw( SerialPort::NotOpen,
           SerialPort::ReadTimeout,
           std::runtime_error )
{
    SerialPort::DataBuffer data_buffer ; 
    //
    // Read a single byte. 
    //
    this->Read( data_buffer, 
                1, 
                msTimeout ) ;
    //
    // If the above Read() does not throw an exception, then we should
    // have exactly one byte in data_buffer.
    //
    assert( 1 == data_buffer.size() ) ;
    //
    return data_buffer[0] ;
}

void
SerialPortImpl::Read( SerialPort::DataBuffer& dataBuffer,
                      const unsigned int      numOfBytes, 
                      const unsigned int      msTimeout ) 
    throw( SerialPort::NotOpen, 
           SerialPort::ReadTimeout, 
           std::runtime_error ) 
{
    //
    // Make sure that the serial port is open. 
    //
    if ( ! this->IsOpen() ) {
        throw SerialPort::NotOpen( ERR_MSG_PORT_NOT_OPEN ) ;
    }
    //
    dataBuffer.empty() ;
    if ( 0 == numOfBytes ) {
        //
        // Read all available data.
        //
        while( this->IsDataAvailable() ) {
            dataBuffer.push_back( ReadByte(msTimeout) ) ;
        }
    } else {
        //
        // Set the read timeout for each byte. 
        //
        if ( 0 == msTimeout ) {
            this->SetReadTimeout( 0,
                                  numOfBytes ) ;
        } else {
            this->SetReadTimeout( msTimeout,
                                  0 ) ;
        }
        //
        bool is_read_failed = false ;
        for(int i=0; i<numOfBytes; ++i) {
            //
            // Read the next byte; keep retrying if EAGAIN error is
            // triggered by the call to read().
            //
            int num_of_bytes_read   = -1 ;
            unsigned char next_byte = 0 ;
            do {
                num_of_bytes_read = read( mFileDescriptor, 
                                          &next_byte, 
                                          1 ) ;
            } while( (num_of_bytes_read < 0) &&
                     (EAGAIN == errno) ) ;
            //
            // Copy the data into dataBuffer if there was no error.
            //
            if ( 1 == num_of_bytes_read ) {
                dataBuffer.push_back( next_byte ) ;
            } else {
                //
                // If num_of_bytes_read is negative, then we had an
                // error while reading. Otherwise, the read timed out.
                // 
                if ( num_of_bytes_read < 0 ) {
                    is_read_failed = true ;
                }
                break ;
            }
        }
        //
        // Check if there was an error or timeout while reading data.
        // 
        if ( is_read_failed ) {
            throw std::runtime_error( strerror(errno) ) ;            
        } 
        //
        // Check if the read timed out. 
        //
        if ( dataBuffer.size() < numOfBytes ) {
            throw SerialPort::ReadTimeout() ;
        }
    }
    return ;
}

void
SerialPortImpl::WriteByte( const unsigned char dataByte )
    throw( SerialPort::NotOpen,
           std::runtime_error ) 
{
    //
    // Make sure that the serial port is open. 
    //
    if ( ! this->IsOpen() ) {
        throw SerialPort::NotOpen( ERR_MSG_PORT_NOT_OPEN ) ;
    }    
    //
    // Write the byte to the serial port. 
    //
    this->Write( &dataByte, 
                 1 ) ;
    return ;
}

void 
SerialPortImpl::Write(const SerialPort::DataBuffer& dataBuffer)
    throw( SerialPort::NotOpen, 
           std::runtime_error ) 
{
    //
    // Make sure that the serial port is open. 
    //
    if ( ! this->IsOpen() ) {
        throw SerialPort::NotOpen( ERR_MSG_PORT_NOT_OPEN ) ;
    }    
    //
    // Nothing needs to be done if there is no data in the buffer.
    //
    if ( 0 == dataBuffer.size() ) {
        return ;
    }
    //
    // Allocate memory for storing the contents of the
    // dataBuffer. This allows us to write all the data using a single
    // call to write() instead of writing one byte at a time.
    //
    unsigned char* local_buffer = new unsigned char[dataBuffer.size()] ;
    if ( 0 == local_buffer ) {
            throw std::runtime_error( std::string(__FUNCTION__) +
                                      ": Cannot allocate memory while writing"
                                      "data to the serial port." ) ;
    }
    //
    // Copy the data into local_buffer.
    //
    std::copy( dataBuffer.begin(), 
               dataBuffer.end(),
               local_buffer ) ;
    //
    // Write data to the serial port. 
    //
    try {
        this->Write( local_buffer, 
                     dataBuffer.size() ) ;
    } catch( ... ) {
        //
        // Free the allocated memory.
        //
        delete [] local_buffer ;
        throw ;
    }
    //
    // Free the allocated memory.
    //
    delete [] local_buffer ;
    return ;
}

void 
SerialPortImpl::Write( const unsigned char* dataBuffer, 
                       const unsigned int   bufferSize )
    throw( SerialPort::NotOpen, 
           std::runtime_error ) 
{
    //
    // Make sure that the serial port is open. 
    //
    if ( ! this->IsOpen() ) {
        throw SerialPort::NotOpen( ERR_MSG_PORT_NOT_OPEN ) ;
    }
    //
    // Write the data to the serial port. Keep retrying if EAGAIN
    // error is received.
    //
    int num_of_bytes_written = -1 ;
    do {
        num_of_bytes_written = write( mFileDescriptor, 
                                      dataBuffer, 
                                      bufferSize ) ;
    } while ( ( num_of_bytes_written < 0 ) &&
              ( EAGAIN == errno ) ) ;
    //
    if ( num_of_bytes_written < 0 ) {
        throw std::runtime_error( strerror(errno) ) ;
    }    
    //
    // :TODO: What happens if num_of_bytes_written < bufferSize ?
    //
    return ;
}

void
SerialPortImpl::SetReadTimeout( const unsigned int msTimeout,
                                const unsigned int numOfBytes ) 
    throw( std::invalid_argument, 
           std::runtime_error ) 
{
    //
    // Read the current serial port settings. 
    //
    termios port_settings ;
    if ( tcgetattr( mFileDescriptor, 
                    &port_settings ) < 0 ) {
        throw std::runtime_error( strerror(errno) ) ;
    }
    //
    // Wait till data is available if msTimeout is zero.
    //
    if ( 0 == msTimeout ) {
        port_settings.c_cc[ VMIN ]  = numOfBytes ;
        port_settings.c_cc[ VTIME ] = 0 ;
    } else {
        //
        // Set the read timeout to the specified number of
        // milliseconds.
        // ------------------------------------------------------------
        // :MAGIC_NUMBER: 100
        // ------------------------------------------------------------
        // VTIME is specified in deciseconds. In order to convert
        // milliseconds to deciseconds, we divide msTimeout by 100.
        //
        port_settings.c_cc[ VMIN ]  = numOfBytes ;
        port_settings.c_cc[ VTIME ] = msTimeout / 100 ;
    }
    //
    // Write the modified settings.
    //
    if ( tcsetattr( mFileDescriptor, 
                    TCSANOW, 
                    &port_settings ) < 0 ) {
        throw std::invalid_argument( strerror(errno) ) ;
    }    
    return ;
}
