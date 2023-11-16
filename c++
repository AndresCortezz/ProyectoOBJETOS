#include <iostream>
#include <vector> 
#include <cctype>
#include <limits>
#include <fstream>
#include <sstream>
#include <random>
#include <ctime>


using namespace std;

void menuPrincipal();
void opcionReservas();
void opcionTrajetas();
int validarEntrada();
string generarToken();
int validarLetraFila(char letra);
int opcionMenu = 0;
string numeroDeReserva;
int cedula;

class Cine {

    int caja = 0;
public:
    void visualizarTotalCaja();
    void verificarCliente();
    
    void actualizarTaquilla(int valor) {
        caja += valor;
    }
    void guardarCajaEnArchivo() const {
        std::ofstream archivo("caja.txt");
        if (archivo.is_open()) {
            archivo << caja;
            archivo.close();
        } else {
            std::cerr << "Error al abrir el archivo para escritura." << std::endl;
        }
    }
    void cargarCajaDesdeArchivo() {
        std::ifstream archivo("caja.txt");
        if (archivo.is_open()) {
            archivo >> caja;
            archivo.close();
        } else {
            std::cerr << "Error al abrir el archivo para lectura." << std::endl;
        }
    }


} reservaCine;

void Cine::visualizarTotalCaja(){
    cout<<"El dinero recaudado es de:  "<<caja<<"$"<<endl;
    opcionMenu = 0;
    menuPrincipal();
}

class Cliente {
protected:
    int cedula;
    int puestosComprados = 0;
    string tokenAcceso;

public:
    Cliente(int);
    int getCedula() const { return cedula; }
    void eliminarReservaPuesto();
    void pagarReservaPuesto();
    int calcularCostoTotal(int cedula);
    
    
};

Cliente::Cliente(int _cedula) {
    cedula = _cedula;

};

class ClienteTarjeta : public Cliente {
private:


public:
    ClienteTarjeta(int cedulaCliente) : Cliente(cedulaCliente) {}
    void comprarPuestoConTarjeta();
    void guardarTarjeta();
  
};


class ClienteNormal : public Cliente {
 
    
public:
    ClienteNormal( int cedulaCliente) : Cliente(cedulaCliente){}
    void comprarTarjetaCinemax(int cedula);
    
    

};
class Tarjetas {
    private:
    int numero;
    int saldo;
public:
    // Constructor
    Tarjetas(int _numero, int _saldo) : numero(_numero), saldo(_saldo) {}

    // Funciones miembro
    void recargarTarjeta();
    void visualizarSaldo();

    int getNumero() const {
        return numero;
    }

    int getSaldo() const {
        return saldo;
    }
    void setSaldo(int nuevoSaldo) {
        saldo = nuevoSaldo;
   }
};


class TarjetasDao {
private:
    std::vector<Tarjetas> tarjetasClientes;
    std::string archivoTarjetas;

public:
    TarjetasDao(std::string archivoTarjetas);
    ~TarjetasDao(); // Destructor para cerrar el archivo al salir del programa
    void guardarTarjetaEnArchivo(int cedula, int saldoInicial);
    void cargarTarjetasDesdeArchivo();
    void crearTarjeta(int cedula, int saldo);
    void mostrarTarjetas();
    bool existeTarjeta(int cedula);
    void recargarTarjeta(int cedula, int monto);
    void descuentoSaldo(int cedula, int valorConDescuento);
    void actualizarArchivo(int cedula, int saldo);
    void visualizarSaldo();

}tarjetasDao("ClientesCinemax.txt");

TarjetasDao::TarjetasDao(std::string _archivoTarjetas) {
    archivoTarjetas = _archivoTarjetas;
}

TarjetasDao::~TarjetasDao() {
    // Cerrar el archivo en el destructor
}
void TarjetasDao::visualizarSaldo() 
{
    int cedula;
    cout<<"digite su numero de cedula"<<endl;
    cedula = validarEntrada();
    
        for (const Tarjetas& tarjeta : tarjetasClientes) {
            if (tarjeta.getNumero() == cedula) {
                std::cout << "El saldo de la tarjeta para la cédula " << cedula << " es: $" << tarjeta.getSaldo() << std::endl;
                return;
            }
        }
        std::cout << "Tarjeta no encontrada para la cédula " << cedula << std::endl;
 }

void TarjetasDao::crearTarjeta(int cedula, int saldoInicial) {
    if (existeTarjeta(cedula)) {
        cout << "Ya existe una tarjeta asociada a este número de cedula." << endl;
    } else {
        Tarjetas nuevaTarjeta(cedula, saldoInicial);
        tarjetasClientes.push_back(nuevaTarjeta);
        cout << "Tarjeta creada exitosamente." << endl;
    }
}
void TarjetasDao::guardarTarjetaEnArchivo(int cedula, int saldoInicial) {
     std::ofstream archivo("ClientesCinemax.txt", std::ios::out);

    // Verificar si el archivo se abrió correctamente
    if (archivo.is_open()) {
        for (const Tarjetas& tarjeta : tarjetasClientes) {
        
                archivo  << tarjeta.getNumero() << "," << tarjeta.getSaldo() << "\n";

    }

        // Cerrar el archivo
        archivo.close();
    } else {
        std::cerr << "Error al abrir el archivo.\n";
    }
    actualizarArchivo( cedula, saldoInicial);
}
void TarjetasDao::actualizarArchivo(int cedulaBuscada, int nuevoSaldo) {
   std::ifstream verificarArchivo(archivoTarjetas);
    bool archivoExiste = verificarArchivo.good();
    verificarArchivo.close();

    std::ifstream archivoEntrada(archivoTarjetas);
    std::ofstream archivoSalida("temp.txt");

    if (!archivoEntrada.is_open() || !archivoSalida.is_open()) {
        std::cerr << "Error al abrir los archivos." << std::endl;
        return;
    }

    bool tarjetaEncontrada = false;

    // Leer y actualizar la información en el archivo temporal
    while (archivoEntrada >> cedula) {
        int saldo;
        archivoEntrada.ignore();  // Ignorar la coma
        archivoEntrada >> saldo;

        if (archivoEntrada.fail()) {
            break;
        }

        if (cedula == cedulaBuscada) {
            archivoSalida << cedula << "," << nuevoSaldo << std::endl;
            tarjetaEncontrada = true;
        } else {
            archivoSalida << cedula << "," << saldo << std::endl;
        }
    }

    archivoEntrada.close();
    archivoSalida.close();

    // Reemplazar el archivo original con el archivo temporal
    std::remove(archivoTarjetas.c_str());
    std::rename("temp.txt", archivoTarjetas.c_str());

    if (!tarjetaEncontrada) {
        std::cerr << "Tarjeta no encontrada." << std::endl;
    } else {
        std::cout << "Tarjeta recargada exitosamente. Nuevo saldo: " << nuevoSaldo << std::endl;
    }
}

void TarjetasDao::mostrarTarjetas() {
    if (tarjetasClientes.empty()) {
        cout << "No hay tarjetas para mostrar." << endl;
        return;
    }

    cout << "Tarjetas registradas:" << endl;

    for (const Tarjetas& tarjeta : tarjetasClientes) {
        
        cout << "Número: " << tarjeta.getNumero() << ", Saldo: " << tarjeta.getSaldo() << endl;
    }
menuPrincipal();
    // Aquí puedes agregar la lógica para mostrar el menú principal o lo que desees hacer después de mostrar las tarjetas
}
void TarjetasDao::cargarTarjetasDesdeArchivo() {
    ifstream archivo("ClientesCinemax.txt"); // Abre el archivo "tarjetas.txt" para lectura

if (archivo.is_open()) { // Verifica si el archivo se abrió correctamente
    tarjetasClientes.clear(); // Borra las tarjetas existentes en el vector

    string linea;

    while (getline(archivo, linea)) {
        istringstream lineaStream(linea);
        string dato;

        int cedula;
        int saldo;

        // Ahora, lee cada dato separado por comas y guárdalos en las variables correspondientes
        if (getline(lineaStream, dato, ',')) {
            cedula = stoi(dato);
        }

        if (getline(lineaStream, dato, ',')) {
            saldo = stoi(dato);
        }

        Tarjetas nuevaTarjeta(cedula, saldo);
        tarjetasClientes.push_back(nuevaTarjeta);
        
    }

    archivo.close();
}
}
bool TarjetasDao::existeTarjeta(int numero)  {
    for (const Tarjetas& tarjeta : tarjetasClientes) {
        if (tarjeta.getNumero() == numero) {
            
            return true; // La tarjeta ya existe
            
        }
    }
    return false; // La tarjeta no existe
}

void TarjetasDao::recargarTarjeta(int cedula, int monto) {
    // Buscar la tarjeta por número de cédula
    for (size_t i = 0; i < tarjetasClientes.size(); ++i) {
        if (tarjetasClientes[i].getNumero() == cedula) {
            // Verificar que el monto sea al menos 50,000
            if (monto < 50000) {
                cout << "El monto mínimo de recarga es 50,000. Operación cancelada." << endl;
                return;
            }

            // Recargar la tarjeta con el monto proporcionado
            int nuevoSaldo = tarjetasClientes[i].getSaldo() + monto;
            tarjetasClientes[i].setSaldo(nuevoSaldo);
            reservaCine.actualizarTaquilla(monto);
            reservaCine.guardarCajaEnArchivo();

            // Guardar la información actualizada en el archivo
            actualizarArchivo(cedula, nuevoSaldo);
            cout << "Tarjeta recargada exitosamente. Nuevo saldo: " << nuevoSaldo << endl;
            return;
        }
    }

    // Si no se encontró la tarjeta
    cout << "No se encontró la tarjeta asociada a la cédula proporcionada." << endl;
}
void TarjetasDao::descuentoSaldo(int cedula, int valorConDescuento)
{
    bool saldoSuficiente = false;
    int saldoActualizado;

    for (size_t i = 0; i < tarjetasClientes.size(); ++i) {
        if (tarjetasClientes[i].getNumero() == cedula && tarjetasClientes[i].getSaldo() >= valorConDescuento) {
            // Actualizar saldo de la tarjeta
             tarjetasClientes[i].setSaldo(tarjetasClientes[i].getSaldo() - valorConDescuento);
             saldoActualizado = tarjetasClientes[i].getSaldo();
            saldoSuficiente = true;
            break;  // Salir del bucle una vez actualizado el saldo
        }
    }

    if (saldoSuficiente) {
        // Resto del código relacionado a la opción de pagar con tarjeta
        // ... (puedes llamar a otras funciones o realizar acciones adicionales aquí)
        cout << "Pago realizado exitosamente." << endl;
        guardarTarjetaEnArchivo(cedula, saldoActualizado );
    } else {
        cout << "Saldo insuficiente en la tarjeta." << endl;
        menuPrincipal();
    }

    // Guardar la tarjeta en el archivo después de salir del bucle
    
}




class Reserva {
private:
    int cedulaCliente;
    char fila;
    int columna;
    string tipoSilla;
    int valorSilla;
    string estadoReserva;
    string tokenAcceso;

public:
    Reserva(int, char, int, string, int, string, string);
    ~Reserva();
    void confirmarReserva();
    void visualizarReserva();
    void cancelarReserva();
    int getCedulaCliente() const {
        return cedulaCliente;
    }

    char getFila() const {
        return fila;
    }

    int getColumna() const {
        return columna;
    }

    string getTipoSilla() const {
        return tipoSilla;
    }

    int getValorSilla() const {
        return valorSilla;
    }

    string getEstadoReserva() const {
        return estadoReserva;
    }
    string getTokenAcceso() const {
        return tokenAcceso;
    }
   void setEstadoReserva(const std::string& nuevoEstado) {
        estadoReserva = nuevoEstado;
   }
   void setValorSilla(float valorDescuento) {
        valorSilla = valorDescuento;
   }

};

Reserva::Reserva(int _cedulaCliente, char _fila, int _columna, string _tipoSilla, int _valorSilla, string _estadoReserva, string _tokenAcceso) {
    cedulaCliente = _cedulaCliente;
    fila = _fila;
    columna = _columna;
    tipoSilla = _tipoSilla;
    valorSilla = _valorSilla;
    estadoReserva = _estadoReserva;
    tokenAcceso = _tokenAcceso;
};
Reserva::~Reserva() {

};

class ReservaDAO {
private:
    vector<Reserva> reservas;
    string archivoReservas;
public:
    ReservaDAO(string archivoReservas);
    void guardarReservasEnArchivo();
    void leerReservasDesdeArchivo(char sala[11][20]);
    void eliminarReservaDeArchivo(string getTokenAcceso);
    void crearReserva(int cedula, char letraFila, int numeroColumna, string tipoSilla, int valor, string estadoReserva, string numeroDeReserva);
    void eliminarReserva(char sala[11][20]);
    int verificarSillasCompradasPorCedula(int cedula);
    void mostrarReservas();
    void confirmarReserva(char sala[11][20]);
    void procesarReservaPagada(char sala[11][20], int cedulaComprarReserva);

} reservaDao("reservas.txt");

ReservaDAO::ReservaDAO(string _archivoReservas) {
    archivoReservas = _archivoReservas;

};
void ReservaDAO::guardarReservasEnArchivo() {
    ofstream archivo("reservas.txt");
   if (archivo.is_open()) {
    for (const Reserva& reserva : reservas)
        archivo << reserva.getCedulaCliente() << "," << reserva.getFila() << "," << reserva.getColumna() << ","
                << reserva.getTipoSilla() << "," << reserva.getValorSilla() << "," << reserva.getEstadoReserva() << ","
                << reserva.getTokenAcceso() << "\n";

    cout << "Reservas guardadas en 'reservas.txt' exitosamente." << endl;
} else cerr << "Error al abrir o crear el archivo para escritura." << endl;
}
void ReservaDAO::leerReservasDesdeArchivo(char sala[11][20]) {
    ifstream archivo("reservas.txt");

    if (!archivo.is_open()) {
        cerr << "No se pudo abrir el archivo 'reservas.txt' para lectura." << endl;
        return;
    }

    reservas.clear();

    string linea;
    while (getline(archivo, linea)) {
        istringstream lineaStream(linea);
        string dato;

        int cedulaCliente, columna, columnaNumero, valorSilla;
        char fila;
        int filaNumero;
        string tipoSilla, estadoReserva, tokenAcceso;

        // Lee cada dato separado por comas y guárdalos en las variables correspondientes
        if (getline(lineaStream, dato, ',')) cedulaCliente = stoi(dato);
        if (getline(lineaStream, dato, ',')) fila = dato[0], filaNumero = validarLetraFila(fila);
        if (getline(lineaStream, dato, ',')) columna = stoi(dato), columnaNumero = columna - 1;
        if (getline(lineaStream, dato, ',')) tipoSilla = dato;
        if (getline(lineaStream, dato, ',')) valorSilla = stoi(dato);
        if (getline(lineaStream, dato, ',')) estadoReserva = dato;
        if (getline(lineaStream, dato, ',')) tokenAcceso = dato;

        Reserva nuevaReserva(cedulaCliente, fila, columna, tipoSilla, valorSilla, estadoReserva, tokenAcceso);
        reservas.push_back(nuevaReserva);
        reservaCine.guardarCajaEnArchivo();

        sala[filaNumero][columnaNumero] = (estadoReserva == "sin pagar") ? 'R' : 'V';
    }

    archivo.close();
    cout << "Reservas cargadas desde 'reservas.txt' exitosamente." << endl;
}

void ReservaDAO::eliminarReservaDeArchivo(string tokenAcceso)
{
    ifstream archivo("reservas.txt"); // Abre el archivo "reservas.txt" para lectura
    ofstream archivoSalida("temp.txt"); // Abre un archivo temporal para escribir

    if (archivo.is_open() && archivoSalida.is_open()) {
        string linea;

        while (getline(archivo, linea)) {
            istringstream lineaStream(linea);
            string dato;

            string numeroDeReserva;
            if (getline(lineaStream, dato, ',')) {
                numeroDeReserva = dato;
            }
            if (numeroDeReserva != tokenAcceso) {
                archivoSalida << linea << "\n";
            }
        }

        archivo.close();
        archivoSalida.close();

        remove("reservas.txt");
        rename("temp.txt", "reservas.txt");

        cout << "Reserva #" << tokenAcceso << " eliminada del archivo exitosamente." << endl;
    }
    else {
        cerr << "Error al abrir los archivos para lectura o escritura." << endl;
    }
}
void ReservaDAO::crearReserva(int cedulaCliente, char letraFila, int numeroColumna, string tipoSilla, int valor, string estadoReserva, string tokenAcceso) {

    Reserva nuevaReserva(cedulaCliente, letraFila, numeroColumna, tipoSilla, valor, estadoReserva, tokenAcceso);

    reservas.push_back(nuevaReserva);

    cout << "Reserva realizada exitosamente." << endl;

}
void ReservaDAO::eliminarReserva(char sala[11][20])
{
    int cedulaCliente;
    int fila;
    int columna;

    cout << "Digite su numero de cedula" << endl;
    cedulaCliente = validarEntrada();
    cout << "Reservas a numero de esta cedula" << endl;
    string tokenAcceso;
    bool tieneReservas = false;
    
    for (size_t i = 0; i < reservas.size(); ++i)
    {
        const Reserva& reserva = reservas[i];
        int verificarCedula = reserva.getCedulaCliente();
        string estadoReserva = reserva.getEstadoReserva();

        if (cedulaCliente == verificarCedula && estadoReserva == "sin pagar")
        {
            cout << "Reserva " << reserva.getTokenAcceso() << ":" << endl;
            cout << "Cedula Cliente: " << reserva.getCedulaCliente() << endl;
            cout << "Fila: " << reserva.getFila() << endl;
            cout << "Columna: " << reserva.getColumna() << endl;
            cout << "Tipo de Silla: " << reserva.getTipoSilla() << endl;
            cout << "Valor de Silla: " << reserva.getValorSilla() << endl;
            cout << "Estado de Reserva: " << reserva.getEstadoReserva() << endl;
            cout << "-------------------------" << endl;
        }
    }

    cout << "Digite el numero de reserva" << endl;
    cin >> tokenAcceso;
    for (size_t i = 0; i < reservas.size(); ++i) {
        const Reserva& reserva = reservas[i];
        if (reserva.getTokenAcceso() == tokenAcceso ) {

            int fila = validarLetraFila(reserva.getFila());
            int columna = reserva.getColumna() - 1;
            string tokenAcceso = reserva.getTokenAcceso();
            int cedulaCliente = reserva.getCedulaCliente();
            string tipo = reserva.getTipoSilla();
            int valor = reserva.getValorSilla();
            string estado = reserva.getEstadoReserva();
            Reserva(cedulaCliente, fila, columna, tipo, valor, estado, tokenAcceso).~Reserva();
            sala[fila][columna] = '*';
            reservas.erase(reservas.begin() + i);
            tieneReservas = true;
            

        }

    }
    if (!tieneReservas) {
            cout << "No se encontró una reserva con el número #" << tokenAcceso << "." << endl;
            
        }
        else {
            reservaDao.eliminarReservaDeArchivo(tokenAcceso);
            reservaDao.guardarReservasEnArchivo();
            
        }
    menuPrincipal();

}

void ReservaDAO::procesarReservaPagada(char sala[11][20], int cedulaComprarReserva) {
    for (Reserva& reserva : reservas) {
    if (cedulaComprarReserva == reserva.getCedulaCliente() && reserva.getEstadoReserva() == "sin pagar") {
               
               
        reserva.setEstadoReserva("Pagada");
        reservaCine.actualizarTaquilla(reserva.getValorSilla());
        reservaCine.guardarCajaEnArchivo();

        int fila = validarLetraFila(reserva.getFila());
        int columna = reserva.getColumna() - 1;
        sala[fila][columna] = 'V';

        cout << "Pago realizado exitosamente." << endl;
    } 
            }

}

void ReservaDAO::mostrarReservas() {
    cout << "Reservas realizadas hasta el momento:" << endl;

    for (size_t i = 0; i < reservas.size(); ++i) {
        const Reserva& reserva = reservas[i];
        cout << "Reserva " << reserva.getTokenAcceso() << ":" << endl;
        cout << "Cedula Cliente: " << reserva.getCedulaCliente() << endl;
        cout << "Fila: " << reserva.getFila() << endl;
        cout << "Columna: " << reserva.getColumna() << endl;
        cout << "Tipo de Silla: " << reserva.getTipoSilla() << endl;
        cout << "Valor de Silla: " << reserva.getValorSilla() << endl;
        cout << "Estado de Reserva: " << reserva.getEstadoReserva() << endl;
        cout << "-------------------------" << endl;
    }
    menuPrincipal();
}
void ReservaDAO::confirmarReserva(char sala[11][20]) {
    int cedulaComprarReserva;
int valorTotal = 0;
int fila, columna;

cout << "Digite su número de cédula" << endl;
cedulaComprarReserva = validarEntrada();

bool reservasEncontradas = false;

for (const Reserva& reserva : reservas) {
    if (cedulaComprarReserva == reserva.getCedulaCliente() && reserva.getEstadoReserva() == "sin pagar") {
        reservasEncontradas = true;

        cout << "Reserva " << reserva.getTokenAcceso() << ":" << endl;
        cout << "Cédula Cliente: " << reserva.getCedulaCliente() << endl;
        cout << "Fila: " << reserva.getFila() << endl;
        cout << "Columna: " << reserva.getColumna() << endl;
        cout << "Tipo de Silla: " << reserva.getTipoSilla() << endl;
        cout << "Valor de Silla: " << reserva.getValorSilla() << endl;
        cout << "Estado de Reserva: " << reserva.getEstadoReserva() << endl;
        cout << "-------------------------" << endl;

        valorTotal += reserva.getValorSilla();
    }
}

if (!reservasEncontradas) {
    cout << "No hay reservas disponibles a este número de cédula" << endl;
    menuPrincipal();
    return;
}

cout << "El valor total es de: " << valorTotal << endl;
cout << "Desea pagar este valor?" << endl;
cout << "1. Si" << endl;
cout << "2. No" << endl;
opcionMenu = validarEntrada();
int opcionPagar = 0;

switch (opcionMenu) {
    case 1: {
        cout << "Como desea pagar" << endl;
        cout << "1. Pagara en efectivo" << endl;
        cout << "2. Pagara con tarjeta" << endl;
        opcionPagar = validarEntrada();

        switch (opcionPagar) {
    case 1:
                procesarReservaPagada(sala, cedulaComprarReserva);
            
        
        guardarReservasEnArchivo();
        break;
    case 2:
    cout << "Ingrese su número de tarjeta:" << endl;
            int numeroTarjeta = validarEntrada();
            float valorConDescuento;


            if (tarjetasDao.existeTarjeta(cedulaComprarReserva)) {
                for (size_t i = 0; i < reservas.size(); ++i) {
    Reserva& reserva = reservas[i];
    if (cedulaComprarReserva == reserva.getCedulaCliente() && reserva.getEstadoReserva() == "sin pagar") {
        // Aplicar descuento del 10%
        float descuento = 0.1 * valorTotal;
        valorConDescuento = valorTotal - descuento;
        reserva.setValorSilla(valorConDescuento);

        cout << "El descuento final es de: " << valorConDescuento << endl;

        // Procesar la reserva pagada
        procesarReservaPagada(sala, cedulaComprarReserva);
        tarjetasDao.descuentoSaldo(cedulaComprarReserva, valorConDescuento);

        // Guardar las reservas actualizadas en el archivo
        guardarReservasEnArchivo();
    }
}
            }
         else {
                cout << "La tarjeta no existe." << endl;
                menuPrincipal();
            }

        
        guardarReservasEnArchivo();
        }

       case 2:  menuPrincipal();
       break;

   
}

opcionMenu = 0;
menuPrincipal();
}
}


class Silla {
private:
    string tipoSilla;
    string estadoSilla;

public:
    string visualizarTipoSilla(bool tipo);
    string visualizarEstadoSilla(bool estado);
};

string Silla::visualizarTipoSilla(bool tipo) {
    string tipoSilla;
    if (tipo) {
        tipoSilla = "Preferencial";
    }
    else {
        tipoSilla = "General";
    }
    return tipoSilla;
}

string Silla::visualizarEstadoSilla(bool estado) {
    string estadoSilla;
    if (estado) {
        estadoSilla = "Ocupada";
    }
    else {
        estadoSilla = "Disponible";
    }
    return estadoSilla;
}

class SillaGeneral : public Silla {
private:
    int precioSillaGeneral;

public:
    int valorSilla();
};

int SillaGeneral::valorSilla() {
    int valorBase = 8000;
    return valorBase;
}

class SillaPreferencial : public Silla {
private:
    int precioSillaPreferencial;

public:
    int valorSilla();
};

int SillaPreferencial::valorSilla() {
    int valorBase = 11000;
    return valorBase;
}


int ReservaDAO::verificarSillasCompradasPorCedula(int cedula) {
    int reservasCompradas = 0;

    for (size_t i = 0; i < reservas.size(); ++i) {
        const Reserva& reserva = reservas[i];
        int verificarCedula = reserva.getCedulaCliente();

        if (cedula == verificarCedula) {

            reservasCompradas++;
        }
    }

    if (reservasCompradas >= 8) {
        cout << "Número de reservas excedido para esta cédula (8)" << endl;
        menuPrincipal();
    }
    return reservasCompradas;
}

class SalaDeCine {
private:
    char salaCine[11][20];
public:
    void crearSalaCine();
    void SeleccionarPuesto();
    void actualizarCine();
    void mostrarSillas();
    void comprobarEstadoSilla(int fila, int columna);
    void liberarPuesto();
    void SillasDeArchivo();
    void pagarPuesto();

};
SalaDeCine SalaDeCine1;

void SalaDeCine::crearSalaCine()
{

    for (int i = 0; i < 11; i++)
    {
        for (int j = 0; j < 20; j++)
        {
            salaCine[i][j] = '*';
        }
    }

}

void SalaDeCine::comprobarEstadoSilla(int fila, int columna)
{

    if (fila >= 0 && fila < 11 && columna >= 0 && columna < 20) {
        if (salaCine[fila][columna] == 'R' ) {
            cout << "Puesto ocupado y pagado" << endl;
            menuPrincipal();
        } else if (salaCine[fila][columna] == '*') {
            cout << "Puesto disponible" << endl;
            salaCine[fila][columna] = 'R'; // Marcar como reservado
        } else {
            cout << "Puesto pagado" << endl;
            salaCine[fila][columna] = 'V'; // Marcar como pagado
        }
    } else {
        cout << "Fila o columna no válida." << endl;
    }
}


void SalaDeCine::mostrarSillas() {
    int numeroFilas = 0;

    cout << "    1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  16  17  18  19  20" << endl;
    cout << "   ---------------------------------------------------------------------------------" << endl;

    for (int i = 0; i < 11; i++) {
        for (int j = 0; j <= 20; j++) {

            if (j == 0) {
                char letraFila = 65 + numeroFilas;
                numeroFilas++;
                cout << letraFila << "  | ";
            }
            else if (j > 0 && j < 10) {
                cout << " | ";
            }
            else if (j >= 10 && j < 20) {
                cout << " | ";
            }
            else {
                cout << " |" << endl;
            }
            if (j != 20) {
                cout << salaCine[i][j];
            }
        }

        cout << "  ---------------------------------------------------------------------------------" << endl;
    }

    cout << " Vacío = *" << endl;
    cout << " Reservado = R" << endl;
    cout << " Vendido = V" << endl;

    cout << "General: sillas en las filas A – H. Costo por boleta de $8,000" << endl;
    cout << "Preferencial: sillas en las filas I – K. Costo por boleta de $11,000" << endl;
}


void SalaDeCine::actualizarCine()
{
    for (int i = 0; i < 20; i++)
    {
        for (int j = 0; j < 11; j++)
        {
            if (salaCine[i][j] != 'R' && salaCine[i][j] != 'V')
            {
                salaCine[i][j] = '*';
            }
        }
    }

}
void SalaDeCine::SeleccionarPuesto()
{

    int numeroDeEntradas;
    int numeroTotalDeEntradasVendidas;
    char letraFila;
    int columna;
    int  numeroColumna;
    int numeroFila;
    string tipoSilla;
    int valor;
    string estadoReserva = "sin pagar";
    string tokenAcceso;

    cout << "Digite el numero de su identificacion" << endl;
    cedula = validarEntrada();

    SalaDeCine1.mostrarSillas();
    SalaDeCine1.actualizarCine();

    do
    {
        int numeroDeEntradasVendidasAnteriormente = reservaDao.verificarSillasCompradasPorCedula(cedula);

        do
        {
            cout << "Digite el numero de entradas que va a comprar" << endl;
            numeroDeEntradas = validarEntrada();
            numeroTotalDeEntradasVendidas = numeroDeEntradas + numeroDeEntradasVendidasAnteriormente;
            if (numeroTotalDeEntradasVendidas > 8)
            {
                cout << "No puede compar mas de 8 entradas" << endl;

            }
        } while (numeroTotalDeEntradasVendidas > 8);


        for (int i = 0; i < numeroDeEntradas; i++)
        {
            do {

                cout << "digite la letra de la fila" << endl;
                cin >> letraFila;

                numeroFila = validarLetraFila(letraFila);


                if (numeroFila >= 0 && numeroFila <= 7) {
                    tipoSilla = "General";
                    valor = SillaGeneral().valorSilla();// Obtener el valor de la silla general
                }
                else {
                    tipoSilla = "Preferencial";
                    valor = SillaPreferencial().valorSilla(); // Obtener el valor de la silla preferencial
                }



            } while (numeroFila < 0 || numeroFila > 11);
            do
            {
                cout << "digite el  numero de la columna" << endl;
                columna = validarEntrada();
                numeroColumna = columna - 1;

                if (numeroColumna < 0 || numeroColumna > 11)
                {
                    cout << "Digite una columna valida" << endl;
                }

            } while (numeroColumna < 0 || numeroColumna > 20);
            SalaDeCine1.comprobarEstadoSilla(numeroFila, numeroColumna);
            tokenAcceso = generarToken();
            reservaDao.crearReserva(cedula, letraFila, columna, tipoSilla, valor, estadoReserva, tokenAcceso);
            reservaDao.guardarReservasEnArchivo();
            SalaDeCine1.mostrarSillas();
        }
        if (numeroTotalDeEntradasVendidas < 8)
        {
            cout << "¿Desea comprar más entradas?" << endl;
            cout << "1. Sí" << endl;
            cout << "2. No (Volver al menú principal)" << endl;

            cin >> opcionMenu;
            switch (opcionMenu)
            {
            case 1:
                break;
            case 2: opcionMenu = 2;
            }
        }
        else {
            opcionMenu = 2;
        }




    } while (opcionMenu == 1);

    menuPrincipal();

}

void SalaDeCine::liberarPuesto()
{


    reservaDao.eliminarReserva(salaCine);

}
void SalaDeCine::pagarPuesto()
{


    reservaDao.confirmarReserva(salaCine);

}
void SalaDeCine::SillasDeArchivo()
{
    reservaDao.leerReservasDesdeArchivo(salaCine);
}

int main()
{

    reservaCine.cargarCajaDesdeArchivo();
    SalaDeCine1.crearSalaCine();
    SalaDeCine1.SillasDeArchivo();
    tarjetasDao.cargarTarjetasDesdeArchivo();
    menuPrincipal();
    reservaCine.guardarCajaEnArchivo();
    return 0;
}

void menuPrincipal()
{
    SalaDeCine1.actualizarCine();
    opcionMenu = 0;
    cout << "Cinemas UAN" << endl;

    cout << "1. Reservar/Cancelar entrada" << endl;
    cout << "2. Recargar/Comprar tarjeta" << endl;
    cout << "3. Pagar reservas" << endl;
    cout << "4. Visualizar dinero de la caja" << endl;
    cout << "5. Visualizar sala de cine" << endl;
    cout << "6. visualizar reservas " << endl;
    cout << "7. visualizar tarjetas " << endl;
    cout << "8. salir " << endl;



    opcionMenu = validarEntrada();

    do
    {

        switch (opcionMenu)
        {
        case 1: opcionReservas();
            break;
        case 2: opcionTrajetas();
        cin.ignore();
            break;
        case 3: SalaDeCine1.pagarPuesto();
            break;
        case 4: reservaCine.visualizarTotalCaja();
        cin.ignore();
        cin.get();
            menuPrincipal();
            break;
        case 5:
           SalaDeCine1.mostrarSillas();
            cin.ignore();
            cin.get();
            menuPrincipal();

            break;
        case 6:  reservaDao.mostrarReservas();

            cin.ignore();
            cin.get();
            menuPrincipal();

            break;
            case 7: tarjetasDao.mostrarTarjetas();
            cin.get();
            menuPrincipal();
            break;
        case 8: exit(1);
        }

    } while (opcionMenu != 9);

}

int validarEntrada()
{
    int entrada;
    while (true) {
        if (cin >> entrada)
        {
            break;  // Sal del bucle si la entrada es un entero válido
        }
        else
        {
            cin.clear(); // Restablece el estado de cin después de un error de entrada
            cin.ignore(numeric_limits<streamsize>::max(), '\n');// Limpia el búfer de entrada
            cout << "Entrada no válida. Inténtalo de nuevo." << endl;
        }
    }
    return entrada;
}
string generarToken() {
    const std::string caracteres = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::mt19937 gen(static_cast<unsigned>(time(0)));
    std::uniform_int_distribution<> distribucion(0, caracteres.size() - 1);

    std::string token;
    for (int i = 0; i < 5; ++i) {  // Generar un token de longitud 10 (puedes ajustar esto)
        token += caracteres[distribucion(gen)];
    }
    return token;
}
int validarLetraFila(char letra)
{
    int numeroFila;
    char validarFila;

    validarFila = towupper(letra);
    numeroFila = static_cast<int>(validarFila) - 65;
    return numeroFila;
}

void opcionReservas()
{

    cout << "Desea realizar una reserva o desea cancelar la reserva" << endl;
    cout << "1. Realizar una reserva" << endl;
    cout << "2. Cancelar la reserva" << endl;
    cout << "3. Volver" << endl;

    opcionMenu = validarEntrada();

    switch (opcionMenu)
    {
    case 1:
        SalaDeCine1.SeleccionarPuesto();

        break;

    case 2:
        SalaDeCine1.liberarPuesto();
        break;
    case 3:
    menuPrincipal();
    break;
    default:
    cout<<"seleccione una opcion valida"<<endl;
    break;
    }

}

void opcionTrajetas()
{
    cout << "Que desea realizar" << endl;
    cout << "1. Comprar una tarjeta " << endl;
    cout << "2. Recargar una tarjeta" << endl;
    cout << "3. Ver saldo" << endl;
    cout << "4. volver" << endl;

    opcionMenu = validarEntrada();

    switch (opcionMenu)
    {
    case 1:
        {
            cout << "Digite su numero de cedula" << endl;
            int cedula = validarEntrada();
            int saldoInicial = 70000;  
            tarjetasDao.crearTarjeta(cedula,saldoInicial); 
            tarjetasDao.guardarTarjetaEnArchivo(cedula,saldoInicial);
            reservaCine.actualizarTaquilla(saldoInicial);
            reservaCine.guardarCajaEnArchivo();
            
            break;
        }

    case 2:
        cout << "Ingrese su número de cédula para recargar la tarjeta: ";
            int cedulaRecarga;
             cedulaRecarga = validarEntrada();

            cout << "Ingrese la cantidad a recargar (mínimo 50,000): ";
            int montoRecarga;
             montoRecarga = validarEntrada();

            tarjetasDao.recargarTarjeta(cedulaRecarga, montoRecarga);
            break;
    case 3:  tarjetasDao.visualizarSaldo();
    break;
    case 4: menuPrincipal();
    break;
    default: cout<<"escoja una opcion"<<endl;
    break;
    }
    menuPrincipal();
}




