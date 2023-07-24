module half_moxon()
{
    $fn=100;
    
    
    // Based on http://on5au.be/content/a10/moxon/moxgen.html
    // Design options
    // All dimensions given in mm
    
    frequency_MHz = 440;
    wire_diameter_mm = 1.7;


    wavelength_mm = 299792.458 / frequency_MHz;
    echo(wavelength_mm);
    wire_diameter_wavelength = wire_diameter_mm / wavelength_mm;
    echo("Wire Diameter in Wavelengths:", wire_diameter_wavelength);
    D1= log(wire_diameter_wavelength);
   
    echo(wire_diameter_wavelength, D1, log(0.000733841));

    if (log(wire_diameter_wavelength) < -6) {
        echo("Wire diameter less than 1E-6 wavelengths:  results uncertain");
    } else if (log(wire_diameter_wavelength) > -2) {
        echo("Wire diameter greater than 1E-2 wavelengths:  results uncertain.");
    }
    
    AA=-.0008571428571;
    AB=-.009571428571;
    AC=.3398571429;
    A=(AA*(D1^2))+(AB*D1)+AC;
    
    BA=-.002142857143;
    BB=-.02035714286;
    BC=.008285714286;
    B=(BA*(D1^2))+(BB*D1)+BC;
    
    CA=.001809523381;
    CB=.01780952381;
    CC=.05164285714;
    C=(CA*(D1^2))+(CB*D1)+CC;
    
    DA=.001;
    DB=.07178571429;
    D=(DA*D1)+DB;
    
    E=(B+C)+D;
    
    echo("Moxon dimensions in wavelengths:")
    echo("A=",A);
    echo("B=",B);
    echo("C=",C);
    echo("D=",D);
    
    echo("Moxon dimensions in mm:")
    echo("A=",A*wavelength_mm);
    echo("B=",B*wavelength_mm);
    echo("C=",C*wavelength_mm);
    echo("D=",D*wavelength_mm);
    
    
    // constants
    c = 299792458; // Speed of sound
    

}


module moxon()
{
    half_moxon();
    mirror([1,0,0]) half_flange();
}

moxon();