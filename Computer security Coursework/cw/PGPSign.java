import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import java.io.BufferedInputStream;
import java.io.DataInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.security.Security;
import java.util.Iterator;

import javax.swing.GroupLayout;
import javax.swing.JApplet;
import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JFileChooser;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JPasswordField;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;
import javax.swing.SwingConstants;

import org.bouncycastle.bcpg.BCPGOutputStream;
import org.bouncycastle.jce.provider.BouncyCastleProvider;
import org.bouncycastle.openpgp.PGPCompressedData;
import org.bouncycastle.openpgp.PGPCompressedDataGenerator;
import org.bouncycastle.openpgp.PGPException;
import org.bouncycastle.openpgp.PGPObjectFactory;
import org.bouncycastle.openpgp.PGPPrivateKey;
import org.bouncycastle.openpgp.PGPPublicKey;
import org.bouncycastle.openpgp.PGPPublicKeyRingCollection;
import org.bouncycastle.openpgp.PGPSecretKey;
import org.bouncycastle.openpgp.PGPSecretKeyRing;
import org.bouncycastle.openpgp.PGPSecretKeyRingCollection;
import org.bouncycastle.openpgp.PGPSignature;
import org.bouncycastle.openpgp.PGPSignatureGenerator;
import org.bouncycastle.openpgp.PGPSignatureList;
import org.bouncycastle.openpgp.PGPUtil;
import org.bouncycastle.openpgp.operator.PBESecretKeyDecryptor;
import org.bouncycastle.openpgp.operator.PGPContentSignerBuilder;
import org.bouncycastle.openpgp.operator.bc.BcPBESecretKeyDecryptorBuilder;
import org.bouncycastle.openpgp.operator.bc.BcPGPContentSignerBuilder;
import org.bouncycastle.openpgp.operator.bc.BcPGPContentVerifierBuilderProvider;
import org.bouncycastle.openpgp.operator.bc.BcPGPDigestCalculatorProvider;


public class PGPSign extends JApplet implements ActionListener, ItemListener{
	private static final long serialVersionUID = 1L;
	/*
	 * Initial parameters and components.
	 */
	JTextArea textview = new JTextArea(25, 60);
	JScrollPane ta = new JScrollPane(textview);
    JButton open = new JButton("Open a File");
    JButton openSignature = new JButton("Open a Signature");
    JButton sign = new JButton("Sign");
    JButton selectkey = new JButton("Open a Private Keystore");
    JButton selectpkey = new JButton("Open a Public Keystore");
    JButton valify = new JButton("Valify");
    String [] userList = {"Paul", "Charles"};
    JComboBox<String> users = new JComboBox<String>(userList);
    JPasswordField pb = new JPasswordField();
    JLabel l1 = new JLabel("Click to select a file:");
    JLabel l2 = new JLabel("User name:");
    JLabel l3 = new JLabel("Enter the Passphrase:");
    JLabel l4 = new JLabel("Click to Sign a file");
    JLabel l5 = new JLabel("Click to varify a signature");
    JLabel lkey = new JLabel("Select your private keyring");
    JLabel lpkey = new JLabel("Select your public keyring");
    JLabel lsig = new JLabel("Click to select signature");
    final JFileChooser fd = new JFileChooser();
    File keyInput = null;
    File document = null;
    File signature = null;
    File pkeyInput = null;

	/*
	 * Main Display component.
	 */
	public void init() {
		try {
			//Preparing the components
			open.addActionListener(this);
			sign.addActionListener(this);
			selectkey.addActionListener(this);
			selectpkey.addActionListener(this);
			selectpkey.setEnabled(false);
			sign.setEnabled(false);
			openSignature.addActionListener(this);
			openSignature.setEnabled(false);
			valify.addActionListener(this);
			valify.setEnabled(false);
			
			//Begin Setting up layouts
			JPanel p = new JPanel();
			GroupLayout layout = new GroupLayout(p);
			p.setLayout(layout);
			layout.setAutoCreateGaps(true);
			layout.setAutoCreateContainerGaps(true);
			layout.setHorizontalGroup(
				layout.createSequentialGroup()
				.addComponent(ta)
				.addGroup(
					layout.createParallelGroup(GroupLayout.Alignment.LEADING)
					.addComponent(l1)
					.addComponent(lkey)
					.addComponent(l2)
					.addComponent(l3)
					.addComponent(l4)
					.addComponent(lsig)
					.addComponent(lpkey)
					.addComponent(l5)
				)
				.addGroup(
					layout.createParallelGroup(GroupLayout.Alignment.LEADING)
					.addComponent(open)
					.addComponent(selectkey)
					.addComponent(users)
					.addComponent(pb)
					.addComponent(sign)
					.addComponent(openSignature)
					.addComponent(selectpkey)
					.addComponent(valify)
				)
			);
			layout.setVerticalGroup(
				layout.createSequentialGroup()
				.addGroup(
					layout.createParallelGroup(GroupLayout.Alignment.BASELINE)
					.addComponent(ta)
					.addGroup(
						layout.createSequentialGroup()
						.addGroup(layout.createParallelGroup().addComponent(l1).addComponent(open))
						.addGroup(layout.createParallelGroup().addComponent(lkey).addComponent(selectkey))
						.addGroup(layout.createParallelGroup().addComponent(l2).addComponent(users))
						.addGroup(layout.createParallelGroup().addComponent(l3).addComponent(pb))
						.addGroup(layout.createParallelGroup().addComponent(l4).addComponent(sign))
						.addGroup(layout.createParallelGroup().addComponent(lsig).addComponent(openSignature))
						.addGroup(layout.createParallelGroup().addComponent(lpkey).addComponent(selectpkey))
						.addGroup(layout.createParallelGroup().addComponent(l5).addComponent(valify))
						
					)
				)
			);
			layout.linkSize(SwingConstants.HORIZONTAL, open, selectkey, users, pb, sign);
			layout.linkSize(SwingConstants.VERTICAL, open, users, pb, sign);
			layout.linkSize(SwingConstants.HORIZONTAL, l1, lkey, l2, l3, l4);
			this.add(p);
			//End Setting up layouts

		}catch(Exception exc){
			exc.printStackTrace();
		}
	}

	
	
	 @SuppressWarnings({ "deprecation", "rawtypes" })
	public void signDocument(File document, File keyring, char[] passPhrase) throws IOException, PGPException {
		 //Mapping user ID with Combo box
		 String userID = null;
		 FileOutputStream out = null;
		 if(document != null){
			 out = new FileOutputStream(document.getName()+".bpg");
		 }
		 if(0 == users.getSelectedIndex()){
			 userID = "Wanchana";
		 }else if(1 == users.getSelectedIndex()){
			 userID = "Charles";
		 }
		 
		 //Retrieving keyring and look for private key by user name
		 Security.addProvider(new BouncyCastleProvider());
		 FileInputStream keyIn = new FileInputStream(keyring);
		 InputStream in = PGPUtil.getDecoderStream(keyIn);
		 PGPSecretKeyRingCollection pgpSec = new PGPSecretKeyRingCollection(in);
		 PGPSecretKey key = null;
		//boolean true was given so just only some part user id need to be matched
		 Iterator rIt = pgpSec.getKeyRings(userID, true); 
		 while(key == null && rIt.hasNext()){
			 PGPSecretKeyRing kRing = (PGPSecretKeyRing)rIt.next();
			 Iterator kIt = kRing.getSecretKeys();
			 while ( key == null && kIt.hasNext() ) {
				 PGPSecretKey k = (PGPSecretKey)kIt.next();
				 if ( k.isSigningKey() ) {
					 key = k;
				 }
			 }
		 }
		 if(key == null){
			 throw new IllegalArgumentException("Can't find key"); 
		 }
		 
		 PBESecretKeyDecryptor decryptor = new BcPBESecretKeyDecryptorBuilder(new BcPGPDigestCalculatorProvider()).build(passPhrase);
		 PGPPrivateKey pgpPrivKey = null;
		 try{
			 pgpPrivKey = key.extractPrivateKey(decryptor);
		 }catch(PGPException exc){
			 throw new PGPException("Passphrase is not matched with User ID");
		 }
	     PGPContentSignerBuilder signerBuilder = new BcPGPContentSignerBuilder(key.getPublicKey().getAlgorithm(),PGPUtil.SHA1);
	     PGPSignatureGenerator sGen = new PGPSignatureGenerator(signerBuilder);
	     sGen.init(PGPSignature.BINARY_DOCUMENT, pgpPrivKey);
	     PGPCompressedDataGenerator cGen = new PGPCompressedDataGenerator(PGPCompressedDataGenerator.ZLIB);
	     BCPGOutputStream bOut = new BCPGOutputStream(cGen.open(out));
	     FileInputStream fIn = new FileInputStream(document);
	     int ch = 0;
	     while ( (ch = fIn.read()) >= 0 ){
	    	 sGen.update((byte)ch); 
	     }
	     sGen.generate().encode(bOut);
	     cGen.close();
	     out.close();
		 keyIn.close();
		 fIn.close();
	 }
	 
	 /*
	  * verify message's signature 
	  */
	 @SuppressWarnings("deprecation")
	public void verifyDocument(File document , File signature, File keyring) throws PGPException, IOException{
		Security.addProvider(new BouncyCastleProvider());
	    FileInputStream fin = new FileInputStream(signature);
	    FileInputStream keyIn = new FileInputStream(keyring);
	    InputStream in = PGPUtil.getDecoderStream(fin);
	    PGPObjectFactory pgpFact = new PGPObjectFactory(in);
	    PGPCompressedData c1 = (PGPCompressedData)pgpFact.nextObject();
	    pgpFact = new PGPObjectFactory(c1.getDataStream());
	    PGPPublicKeyRingCollection pgpPubRingCollection =new PGPPublicKeyRingCollection(PGPUtil.getDecoderStream(keyIn));
	    InputStream dIn = new FileInputStream(document);
	    int ch;
	    PGPSignatureList p3 = (PGPSignatureList)pgpFact.nextObject();
	    PGPSignature sig = p3.get(0);
	    PGPPublicKey key = pgpPubRingCollection.getPublicKey(sig.getKeyID());
	    BcPGPContentVerifierBuilderProvider bc = new BcPGPContentVerifierBuilderProvider();
	    sig.init(bc, key);
	    while ((ch = dIn.read()) >= 0) { sig.update((byte)ch); }
	    if (sig.verify()) {
	    	dIn.close();
	    	throw new PGPException("signature verified.");
	    } else {
	    	dIn.close();
	    	throw new PGPException("signature verification failed.");
	    }
	 }
	
 	/*
	 * Open files explorer.
	 */
	public File openFileDialog(){
		File file = null;
	    int returnVal = fd.showOpenDialog(this);
	    if (returnVal == JFileChooser.APPROVE_OPTION) {
	    	file = fd.getSelectedFile();
	    }
		return file;
	}
	
	/*
	 * Event listener for Buttons.
	 */
	@Override
	public void actionPerformed(ActionEvent e) {
		// TODO Auto-generated method stub
		
		try{
			if (e.getSource() == open) {
			//open a file button
				document = openFileDialog();
				if(document != null){
					FileInputStream fis = new FileInputStream(document);
					BufferedInputStream bis = new BufferedInputStream(fis);
			    	DataInputStream dis = new DataInputStream(bis);
			    	int len = (int) document.length();
			    	byte[] buf = new byte[len];
			    	len = dis.read(buf, 0, len);
			    	fis.close();
			    	textview.setText(new String(buf, 0, len));
				}
			}else if(e.getSource() == selectkey){
			//open a keystore button
				keyInput = openFileDialog();
				if(keyInput != null){
					sign.setEnabled(true);
				}else{
					throw new Exception("Please select Keystore file.");
				}
			}else if(e.getSource() == sign){
			// sign button
				char[] passPhrase = pb.getPassword();
				if(document != null){
					if(keyInput != null){
						signDocument(document, keyInput, passPhrase);
						openSignature.setEnabled(true);
						selectpkey.setEnabled(true);
					}else{
						throw new Exception("Please select Keystore file.");
					}
				}else{
					throw new Exception("Pless select the target document.");
				}
			}else if(e.getSource() == openSignature){
			//open a signature
				signature = openFileDialog();
				if(signature != null){
					FileInputStream fis = new FileInputStream(signature);
					BufferedInputStream bis = new BufferedInputStream(fis);
			    	DataInputStream dis = new DataInputStream(bis);
			    	int len = (int) signature.length();
			    	byte[] buf = new byte[len];
			    	len = dis.read(buf, 0, len);
			    	fis.close();
			    	textview.setText(new String(buf, 0, len));
				}
			}else if(e.getSource() == selectpkey){
			//open a public keystore
				pkeyInput = openFileDialog();
				if(pkeyInput != null){
					valify.setEnabled(true);
				}else{
					throw new Exception("Please select public Keystore file.");
				}
			}else if(e.getSource() == valify){
				if(signature != null){
					if(keyInput != null){
						verifyDocument(document, signature, pkeyInput);
					}else{
						throw new Exception("Please select Keystore file.");
					}
				}else{
					throw new Exception("Pless select the target signature.");
				}
			}
		}catch(Exception exc){
			JOptionPane.showMessageDialog(null, exc.getMessage(), "Error",JOptionPane.ERROR_MESSAGE);
		}
	}
	/*
	 * Event listener for Combo box.
	 */
	@Override
	public void itemStateChanged(ItemEvent e) {
		// TODO Auto-generated method stub
		
	}
}